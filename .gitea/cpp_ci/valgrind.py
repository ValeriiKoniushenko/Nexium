#!/usr/bin/env python3
"""Run the test suite under Valgrind and publish a replaceable Gitea review."""

from __future__ import annotations

import argparse
import os
import re
import subprocess
import sys
from dataclasses import dataclass

from .gitea_client import GiteaClient, review_marker


VALGRIND_ERROR_EXIT_CODE = 42
MAX_REVIEW_OUTPUT_CHARS = 8_000
ERROR_SUMMARY_RE = re.compile(r"ERROR SUMMARY: (?P<count>\d+) errors?")
DEFINITE_LEAK_RE = re.compile(r"definitely lost: (?P<bytes>[\d,]+) bytes")


def valgrind_command() -> str:
    """Allow the shared toolchain configuration to select Valgrind."""
    return os.environ.get("CPP_CI_VALGRIND", "valgrind")


@dataclass
class ValgrindResult:
    command: list[str]
    returncode: int
    stdout: str
    stderr: str

    @property
    def failed(self) -> bool:
        return self.returncode != 0

    @property
    def infrastructure_error(self) -> bool:
        return self.returncode == 127 or "Fatal error at startup" in self.stderr

    @property
    def check_state(self) -> str:
        if self.infrastructure_error:
            return "error"
        return "failure" if self.failed else "success"


def run_valgrind(
        executable: str,
        test_args: list[str],
        suppressions: list[str] | tuple[str, ...] = (),
        *,
        generate_suppressions: bool = False,
) -> ValgrindResult:
    command = [
        valgrind_command(),
        "--leak-check=full",
        "--show-leak-kinds=definite",
        "--errors-for-leak-kinds=definite",
        "--track-origins=yes",
        *( ["--gen-suppressions=all"] if generate_suppressions else [] ),
        *(f"--suppressions={suppression}" for suppression in suppressions),
        f"--error-exitcode={VALGRIND_ERROR_EXIT_CODE}",
        executable,
        *test_args,
    ]

    try:
        completed = subprocess.run(command, capture_output=True, text=True)
    except OSError as error:
        return ValgrindResult(command, 127, "", f"Unable to start Valgrind: {error}")

    return ValgrindResult(
        command,
        completed.returncode,
        completed.stdout,
        completed.stderr,
    )


def result_description(result: ValgrindResult) -> str:
    if not result.failed:
        return "Valgrind clean"
    if result.infrastructure_error:
        return f"Valgrind could not run (exit code {result.returncode})"

    error_match = ERROR_SUMMARY_RE.search(result.stderr)
    leak_match = DEFINITE_LEAK_RE.search(result.stderr)
    details = []

    if error_match and error_match.group("count") != "0":
        details.append(f"{error_match.group('count')} error(s)")
    if leak_match and leak_match.group("bytes") != "0":
        details.append(f"{leak_match.group('bytes')} byte(s) definitely lost")

    summary = ", ".join(details) or f"exit code {result.returncode}"
    return f"❗️Valgrind failed: {summary}"


def tail_for_review(text: str, limit: int = MAX_REVIEW_OUTPUT_CHARS) -> str:
    text = text.strip().replace("```", "''' ")
    if len(text) <= limit:
        return text
    return f"[output truncated to last {limit} characters]\n{text[-limit:]}"


def failure_review_body(result: ValgrindResult) -> str:
    sections = []
    if result.stdout.strip():
        sections.append(f"Test output:\n{result.stdout.strip()}")
    if result.stderr.strip():
        sections.append(f"Valgrind output:\n{result.stderr.strip()}")

    output = tail_for_review("\n\n".join(sections))
    command = " ".join(result.command)
    title = "Valgrind could not run" if result.infrastructure_error else "Valgrind failed"
    return (
        f"**{title}** (exit code `{result.returncode}`).\n\n"
        f"Command: `{command}`\n\n"
        "<details>\n"
        "<summary>Valgrind output</summary>\n\n"
        f"```text\n{output}\n```\n\n"
        "</details>"
    )


def publish_result(
    client: GiteaClient,
    result: ValgrindResult,
    *,
    dry_run: bool,
    verbose: bool,
) -> None:
    pr_number = GiteaClient.resolve_pr_number()
    sha = GiteaClient.resolve_sha() or ""
    marker = review_marker("valgrind")
    description = result_description(result)

    if pr_number is None:
        if verbose:
            print("[gitea] not a pull_request event - skipping review comment")
        if sha and not dry_run:
            try:
                client.publish_check(
                    sha,
                    result.check_state,
                    "valgrind",
                    description,
                )
            except Exception as error:
                print(f"[gitea] failed to publish check: {error}", file=sys.stderr)
        return

    if dry_run:
        print(f"[dry-run] would remove previous Valgrind review on PR #{pr_number}")
        if result.failed:
            print(f"[dry-run] would create Valgrind failure review on PR #{pr_number}")
        return

    try:
        client.dismiss_previous_reviews(pr_number, marker=marker)
    except Exception as error:
        print(f"[gitea] failed to clear previous Valgrind review: {error}", file=sys.stderr)

    if result.failed:
        try:
            client.create_review(
                pr_number,
                body=failure_review_body(result),
                event="COMMENT",
                commit_id=sha,
                marker=marker,
            )
        except Exception as error:
            print(f"[gitea] failed to create Valgrind review: {error}", file=sys.stderr)

    if sha:
        try:
            client.publish_check(
                sha,
                result.check_state,
                "valgrind",
                description,
            )
        except Exception as error:
            print(f"[gitea] failed to publish check: {error}", file=sys.stderr)


def main(
        *,
        default_executable: str = "build/bin/Tests",
        suppressions: str | list[str] | tuple[str, ...] | None = None,
) -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--executable",
        default=default_executable,
        help=f"test executable to run (default: {default_executable})",
    )
    parser.add_argument(
        "--dry-run",
        action="store_true",
        help="run Valgrind but do not change Gitea reviews or statuses",
    )
    parser.add_argument(
        "--no-gitea",
        action="store_true",
        help="skip Gitea review and commit-status API calls",
    )
    parser.add_argument("--verbose", "-v", action="store_true")
    parser.add_argument(
        "--suppression",
        action="append",
        default=[],
        help="Valgrind suppression file; can be repeated",
    )
    parser.add_argument(
        "--generate-suppressions",
        action="store_true",
        help="include copyable suppression blocks for every unsuppressed finding",
    )
    parser.add_argument(
        "test_args",
        nargs=argparse.REMAINDER,
        help="arguments passed to the test executable after '--'",
    )
    args = parser.parse_args()

    default_suppressions: list[str]
    if suppressions is None:
        default_suppressions = []
    elif isinstance(suppressions, str):
        default_suppressions = [suppressions]
    else:
        default_suppressions = list(suppressions)
    all_suppressions = [*default_suppressions, *args.suppression]

    if not os.path.isfile(args.executable):
        print(f"[error] test executable not found: {args.executable}", file=sys.stderr)
        sys.exit(2)
    for suppression in all_suppressions:
        if not os.path.isfile(suppression):
            print(f"[error] Valgrind suppression file not found: {suppression}", file=sys.stderr)
            sys.exit(2)

    test_args = args.test_args
    if test_args and test_args[0] == "--":
        test_args = test_args[1:]

    result = run_valgrind(
        args.executable,
        test_args,
        all_suppressions,
        generate_suppressions=args.generate_suppressions,
    )

    if args.verbose:
        print(f"[debug] running: {' '.join(result.command)}")
    if result.stdout:
        print(result.stdout, end="" if result.stdout.endswith("\n") else "\n")
    if result.stderr:
        print(
            result.stderr,
            file=sys.stderr,
            end="" if result.stderr.endswith("\n") else "\n",
        )

    if not args.no_gitea:
        client = GiteaClient.from_env(verbose=args.verbose)
        if client is not None:
            publish_result(
                client,
                result,
                dry_run=args.dry_run,
                verbose=args.verbose,
            )
        elif args.verbose:
            print("[gitea] client not configured - skipping review publish")

    print(result_description(result))
    if result.failed:
        sys.exit(1)


if __name__ == "__main__":
    main()
