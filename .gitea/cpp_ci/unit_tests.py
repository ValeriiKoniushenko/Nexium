#!/usr/bin/env python3
"""Run GoogleTest, summarize failures, and publish a replaceable Gitea review."""

from __future__ import annotations

import argparse
import json
import os
import re
import subprocess
import sys
from dataclasses import dataclass

from .gitea_client import GiteaClient, review_marker


MAX_REVIEW_OUTPUT_CHARS = 16_000
FAILED_TEST_RE = re.compile(
    r"^\[  FAILED  \] (?P<name>[^\s]+)(?: \(\d+ ms\))?$",
    re.MULTILINE,
)
RUN_TEST_RE = re.compile(r"^\[ RUN      \] (?P<name>[^\s]+)$")
FAILURE_START_RE = re.compile(r"^(?:.+:\d+|unknown file): Failure$")
LOCATION_RE = re.compile(
    r"^(?P<path>.+\.(?:c|cc|cpp|cxx|h|hh|hpp|hxx)):(?P<line>\d+): Failure$",
    re.MULTILINE,
)
PASSED_COUNT_RE = re.compile(
    r"^\[  PASSED  \] (?P<count>\d+) tests?\.$",
    re.MULTILINE,
)
FAILED_COUNT_RE = re.compile(
    r"^\[  FAILED  \] (?P<count>\d+) tests?, listed below:$",
    re.MULTILINE,
)


@dataclass(frozen=True)
class FailureLocation:
    path: str
    line: int


@dataclass
class UnitTestResult:
    command: list[str]
    returncode: int
    output: str
    failed_tests: list[str]
    locations: list[FailureLocation]
    passed_count: int | None
    failed_count: int | None

    @property
    def failed(self) -> bool:
        return self.returncode != 0

    @property
    def infrastructure_error(self) -> bool:
        return self.returncode == 127

    @property
    def check_state(self) -> str:
        if self.infrastructure_error:
            return "error"
        return "failure" if self.failed else "success"


def _deduplicate(values: list[str]) -> list[str]:
    return list(dict.fromkeys(values))


def _normalize_source_path(path: str) -> str:
    normalized = path.replace("\\", "/")
    repo_root = os.path.abspath(os.getcwd()).replace("\\", "/")
    if normalized.startswith(f"{repo_root}/"):
        return normalized[len(repo_root) + 1:]

    for prefix in ("tests/", "sources/"):
        marker = f"/{prefix}"
        if marker in normalized:
            return f"{prefix}{normalized.split(marker, 1)[1]}"
    return normalized


def _parse_count(pattern: re.Pattern[str], output: str) -> int | None:
    match = pattern.search(output)
    return int(match.group("count")) if match else None


def parse_result(command: list[str], returncode: int, output: str) -> UnitTestResult:
    failed_tests = _deduplicate(
        [match.group("name") for match in FAILED_TEST_RE.finditer(output)]
    )
    locations = []
    seen_locations = set()
    for match in LOCATION_RE.finditer(output):
        location = FailureLocation(
            _normalize_source_path(match.group("path")),
            int(match.group("line")),
        )
        if location not in seen_locations:
            seen_locations.add(location)
            locations.append(location)

    return UnitTestResult(
        command=command,
        returncode=returncode,
        output=output,
        failed_tests=failed_tests,
        locations=locations,
        passed_count=_parse_count(PASSED_COUNT_RE, output),
        failed_count=_parse_count(FAILED_COUNT_RE, output),
    )


def run_unit_tests(executable: str, test_args: list[str]) -> UnitTestResult:
    command = [executable, *test_args]
    try:
        completed = subprocess.run(
            command,
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            text=True,
            errors="replace",
        )
    except OSError as error:
        return parse_result(command, 127, f"Unable to start unit tests: {error}\n")

    return parse_result(command, completed.returncode, completed.stdout)


def result_description(result: UnitTestResult, label: str) -> str:
    if not result.failed:
        passed = f": {result.passed_count} passed" if result.passed_count is not None else ""
        return f"{label} unit tests passed{passed}"
    if result.infrastructure_error:
        return f"{label} unit tests could not run"

    failed_count = result.failed_count or len(result.failed_tests)
    suffix = f": {failed_count} failed" if failed_count else f": exit code {result.returncode}"
    return f"{label} unit tests failed{suffix}"


def _failure_blocks(output: str) -> list[str]:
    return [
        block
        for blocks in failure_blocks_by_test(output).values()
        for block in blocks
    ]


def _normalize_failure_block(block: str) -> str:
    def replace_location(match: re.Match[str]) -> str:
        return (
            f"{_normalize_source_path(match.group('path'))}:"
            f"{match.group('line')}: Failure"
        )

    return LOCATION_RE.sub(replace_location, block).strip()


def failure_blocks_by_test(output: str) -> dict[str, list[str]]:
    blocks: dict[str, list[str]] = {}
    current_test = ""
    current_block: list[str] | None = None

    for line in output.splitlines():
        run_match = RUN_TEST_RE.match(line)
        if run_match:
            current_test = run_match.group("name")
            current_block = None
            continue

        if current_test and current_block is None and FAILURE_START_RE.match(line):
            current_block = [line]
            continue

        if current_block is not None:
            current_block.append(line)

        failed_match = FAILED_TEST_RE.match(line)
        if failed_match and failed_match.group("name") == current_test:
            if current_block:
                assertion_lines = current_block[:-1]
                block = _normalize_failure_block("\n".join(assertion_lines))
                test_blocks = blocks.setdefault(current_test, [])
                if block not in test_blocks:
                    test_blocks.append(block)
            current_test = ""
            current_block = None

    return blocks


def output_for_review(output: str, limit: int = MAX_REVIEW_OUTPUT_CHARS) -> str:
    sanitized = output.strip().replace("```", "''' ")
    if len(sanitized) <= limit:
        return sanitized

    blocks = _failure_blocks(sanitized)
    details = "\n\n".join(blocks)
    tail_limit = max(2_000, limit - len(details) - 100)
    tail = sanitized[-tail_limit:]
    return (
        "[output shortened; see the CI job for the complete log]\n\n"
        f"{details}\n\nFinal output:\n{tail}"
    )[-limit:]


def failure_review_body(result: UnitTestResult, label: str) -> str:
    failed_tests = result.failed_tests or ["GoogleTest process failed before reporting a test name"]
    test_list = "\n".join(f"- `{name}`" for name in failed_tests)

    location_section = ""
    if result.locations:
        locations = "\n".join(
            f"- `{location.path}:{location.line}`" for location in result.locations
        )
        location_section = f"\n\nFailure locations:\n{locations}"

    command = " ".join(result.command)
    output = output_for_review(result.output)
    return (
        f"**Unit tests failed: {label}** (exit code `{result.returncode}`).\n\n"
        f"Failed tests:\n{test_list}{location_section}\n\n"
        f"Command: `{command}`\n\n"
        "<details>\n"
        "<summary>GoogleTest output</summary>\n\n"
        f"```text\n{output}\n```\n\n"
        "</details>"
    )


def publish_result(
        client: GiteaClient,
        result: UnitTestResult,
        *,
        label: str,
        context: str,
        dry_run: bool,
        verbose: bool,
) -> None:
    pr_number = GiteaClient.resolve_pr_number()
    sha = GiteaClient.resolve_sha() or ""
    marker = review_marker(context)
    description = result_description(result, label)

    if pr_number is None:
        if verbose:
            print("[gitea] not a pull_request event - skipping unit-test review")
        if sha and not dry_run:
            try:
                client.publish_check(sha, result.check_state, context, description)
            except Exception as error:
                print(f"[gitea] failed to publish unit-test check: {error}", file=sys.stderr)
        return

    if dry_run:
        print(f"[dry-run] would replace unit-test review on PR #{pr_number}")
        if result.failed:
            print(failure_review_body(result, label))
        return

    try:
        client.dismiss_previous_reviews(pr_number, marker=marker)
    except Exception as error:
        print(f"[gitea] failed to clear previous unit-test review: {error}", file=sys.stderr)

    if result.failed:
        try:
            client.create_review(
                pr_number,
                body=failure_review_body(result, label),
                event="COMMENT",
                commit_id=sha,
                marker=marker,
            )
        except Exception as error:
            print(f"[gitea] failed to create unit-test review: {error}", file=sys.stderr)

    if sha:
        try:
            client.publish_check(sha, result.check_state, context, description)
        except Exception as error:
            print(f"[gitea] failed to publish unit-test check: {error}", file=sys.stderr)


def write_step_summary(result: UnitTestResult, label: str) -> None:
    summary_path = os.environ.get("GITHUB_STEP_SUMMARY")
    if not summary_path:
        return

    status = "passed" if not result.failed else "failed"
    details = []
    if result.passed_count is not None:
        details.append(f"{result.passed_count} passed")
    failed_count = result.failed_count or len(result.failed_tests)
    if failed_count:
        details.append(f"{failed_count} failed")
    counts = f" ({', '.join(details)})" if details else ""

    with open(summary_path, "a") as summary:
        summary.write(f"## Unit tests: {label}\n\n{status.capitalize()}{counts}.\n")


def write_json_report(result: UnitTestResult, label: str, report_path: str) -> None:
    parent = os.path.dirname(report_path)
    if parent:
        os.makedirs(parent, exist_ok=True)

    payload = {
        "schema_version": 1,
        "label": label,
        "command": result.command,
        "returncode": result.returncode,
        "output": result.output,
    }
    with open(report_path, "w") as report:
        json.dump(payload, report, indent=2)
        report.write("\n")


def main(*, default_executable: str = "build/bin/Tests") -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--executable",
        default=default_executable,
        help=f"test executable to run (default: {default_executable})",
    )
    parser.add_argument("--label", required=True, help="human-readable build variant")
    parser.add_argument("--context", required=True, help="unique Gitea review/check context")
    parser.add_argument(
        "--report-path",
        default="",
        help="write a machine-readable result for aggregate reporting",
    )
    parser.add_argument("--dry-run", action="store_true")
    parser.add_argument("--no-gitea", action="store_true")
    parser.add_argument("--verbose", "-v", action="store_true")
    parser.add_argument(
        "test_args",
        nargs=argparse.REMAINDER,
        help="arguments passed to the test executable after '--'",
    )
    args = parser.parse_args()

    test_args = args.test_args
    if test_args and test_args[0] == "--":
        test_args = test_args[1:]

    result = run_unit_tests(args.executable, test_args)
    if args.verbose:
        print(f"[debug] running: {' '.join(result.command)}")
    if result.output:
        print(result.output, end="" if result.output.endswith("\n") else "\n")

    write_step_summary(result, args.label)
    if args.report_path:
        write_json_report(result, args.label, args.report_path)

    if not args.no_gitea:
        client = GiteaClient.from_env(verbose=args.verbose)
        if client is not None:
            publish_result(
                client,
                result,
                label=args.label,
                context=args.context,
                dry_run=args.dry_run,
                verbose=args.verbose,
            )
        elif args.verbose:
            print("[gitea] client not configured - skipping unit-test review")

    print(result_description(result, args.label))
    if result.failed:
        sys.exit(result.returncode if result.returncode > 0 else 1)
