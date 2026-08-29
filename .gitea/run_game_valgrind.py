#!/usr/bin/env python3
"""Run a game under Valgrind briefly and publish a replaceable PR review."""

from __future__ import annotations

import argparse
import re
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path

from capture_game_ui import (
    DISPLAY,
    executable_key,
    get_headless_environment,
    get_screen_size,
    markdown_inline,
    terminate,
    wait_for_xvfb,
)
from gitea_client import GiteaClient, review_marker

ERROR_SUMMARY_RE = re.compile(r"ERROR SUMMARY: (?P<count>\d+) errors?")
MAX_OUTPUT_CHARS = 8_000
TIMEOUT_EXIT_CODE = 124


@dataclass
class ValgrindGameResult:
    command: list[str]
    returncode: int
    stdout: str
    stderr: str
    timeout_seconds: float

    @property
    def error_count(self) -> int | None:
        matches = list(ERROR_SUMMARY_RE.finditer(self.stderr))
        return int(matches[-1].group("count")) if matches else None

    @property
    def timed_out(self) -> bool:
        return self.returncode == TIMEOUT_EXIT_CODE

    @property
    def infrastructure_error(self) -> bool:
        return self.returncode == 127 or "Fatal error at startup" in self.stderr

    @property
    def has_errors(self) -> bool:
        return self.error_count is not None and self.error_count != 0

    @property
    def failed(self) -> bool:
        return (
            self.infrastructure_error
            or self.error_count is None
            or self.has_errors
            or not self.timed_out
        )

    @property
    def check_state(self) -> str:
        if self.infrastructure_error or self.error_count is None:
            return "error"
        return "failure" if self.failed else "success"


def run_game_valgrind(executable: Path, timeout_seconds: float) -> ValgrindGameResult:
    command = [
        "timeout",
        "--signal=TERM",
        "--kill-after=5s",
        f"{timeout_seconds:g}s",
        "valgrind",
        "--leak-check=full",
        "--show-leak-kinds=definite",
        "--errors-for-leak-kinds=definite",
        "--track-origins=yes",
        "--error-exitcode=42",
        # "--gen-suppressions=all",
        "--suppressions=valgrind.supp",
        str(executable.resolve()),
    ]
    xvfb: subprocess.Popen[object] | None = None
    try:
        environment = get_headless_environment()
        screen_width, screen_height = get_screen_size()
        xvfb = subprocess.Popen(
            [
                "Xvfb",
                DISPLAY,
                "-screen",
                "0",
                f"{screen_width}x{screen_height}x24",
                "+extension",
                "GLX",
                "+iglx",
                "-nolisten",
                "tcp",
            ],
            stdout=subprocess.DEVNULL,
            stderr=subprocess.DEVNULL,
        )
        wait_for_xvfb(xvfb, DISPLAY)
        completed = subprocess.run(
            command,
            cwd=Path.cwd(),
            env=environment,
            capture_output=True,
            text=True,
        )
    except (OSError, RuntimeError) as error:
        return ValgrindGameResult(command, 127, "", str(error), timeout_seconds)
    finally:
        if xvfb is not None:
            terminate(xvfb, "Xvfb")

    return ValgrindGameResult(
        command,
        completed.returncode,
        completed.stdout,
        completed.stderr,
        timeout_seconds,
    )


def tail_for_review(text: str) -> str:
    text = text.strip().replace("```", "''' ")
    if len(text) <= MAX_OUTPUT_CHARS:
        return text
    return f"[output truncated to last {MAX_OUTPUT_CHARS} characters]\n{text[-MAX_OUTPUT_CHARS:]}"


def result_description(result: ValgrindGameResult) -> str:
    if result.infrastructure_error:
        return "Game Valgrind could not run"
    if result.error_count is None:
        return "Game Valgrind did not produce an error summary"
    if result.has_errors:
        return f"Game Valgrind found {result.error_count} errors"
    if not result.timed_out:
        return f"Game exited before the {result.timeout_seconds:g}s limit (exit code {result.returncode})"
    return "Game Valgrind clean"


def review_body(executable_name: str, result: ValgrindGameResult) -> str:
    error_count = "unavailable" if result.error_count is None else str(result.error_count)
    status = "Clean"
    if result.infrastructure_error or result.error_count is None or not result.timed_out:
        status = "Run failed"
    elif result.has_errors:
        status = "Issues found"

    output = tail_for_review("\n\n".join(part for part in (result.stdout, result.stderr) if part))
    lines = [
        f"## Valgrind run of _{markdown_inline(executable_name)}_",
        "",
    ]
    if result.has_errors:
        lines.extend(
            (
                f"> **Valgrind issues found:** `{result.error_count}` errors.",
                "",
            )
        )
    elif status == "Run failed":
        lines.extend((f"> **Valgrind run failed:** {markdown_inline(result_description(result))}.", ""))
    else:
        lines.extend(("> **Valgrind clean:** no errors.", ""))
    lines.extend(
        (
            "| Metric | Value |",
            "| --- | --- |",
            f"| Time limit | `{result.timeout_seconds:g}s` |",
            f"| Process status | `{status}` |",
            f"| Valgrind errors | `{error_count}` |",
            f"| Timeout reached | `{'yes' if result.timed_out else 'no'}` |",
            f"| Command exit code | `{result.returncode}` |",
            "",
            "<details>",
            "<summary><b>Valgrind output</b></summary>",
            "",
            "```text",
            output or "No Valgrind output.",
            "```",
            "",
            "</details>",
        )
    )
    return "\n".join(lines)


def publish_result(client: GiteaClient, executable_name: str, result: ValgrindGameResult) -> None:
    pr_number = GiteaClient.resolve_pr_number()
    sha = GiteaClient.resolve_sha() or ""
    check_context = f"{executable_key(executable_name)}-valgrind"
    description = result_description(result)
    if pr_number is None:
        if sha:
            client.publish_check(sha, result.check_state, check_context, description)
        return

    marker = review_marker(check_context)
    try:
        client.dismiss_previous_reviews(pr_number, marker=marker)
    except Exception as error:
        print(f"[gitea] failed to clear previous Valgrind review: {error}", file=sys.stderr)

    if result.has_errors:
        client.create_review(
            pr_number,
            body=review_body(executable_name, result),
            event="COMMENT",
            commit_id=sha,
            marker=marker,
        )
    if sha:
        client.publish_check(sha, result.check_state, check_context, description)


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--executable", default="build/bin/TemplateGame", help="game executable to run")
    parser.add_argument("--timeout", type=float, default=5.0, help="maximum game runtime in seconds")
    parser.add_argument("--no-gitea", action="store_true")
    args = parser.parse_args()

    executable = Path(args.executable)
    if not executable.is_file():
        parser.error(f"game executable not found: {executable}")
    if args.timeout <= 0:
        parser.error("--timeout must be greater than zero")
    result = run_game_valgrind(executable, args.timeout)
    if result.stdout:
        print(result.stdout, end="" if result.stdout.endswith("\n") else "\n")
    if result.stderr:
        print(result.stderr, file=sys.stderr, end="" if result.stderr.endswith("\n") else "\n")

    if not args.no_gitea:
        client = GiteaClient.from_env()
        if client is not None:
            try:
                publish_result(client, executable.name, result)
            except Exception as error:
                print(f"[error] failed to publish game Valgrind result: {error}", file=sys.stderr)
                sys.exit(1)

    print(result_description(result))
    if result.failed:
        sys.exit(1)


if __name__ == "__main__":
    main()
