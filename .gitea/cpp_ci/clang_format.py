#!/usr/bin/env python3
"""
Checks that changed C/C++ files are clang-format compliant.

CI usage (unchanged behavior):
    python3 .gitea/check_clang_format.py

Local/debug usage from your machine:
    python3 .gitea/check_clang_format.py --base develop --verbose
    python3 .gitea/check_clang_format.py --base develop --fix          # auto-apply formatting
    python3 .gitea/check_clang_format.py --files src/foo.cpp src/bar.h
    python3 .gitea/check_clang_format.py --base develop --dry-run      # show diff, don't write report/exit 1
"""

import argparse
import hashlib
import json
import os
import subprocess
import sys

from .gitea_client import GiteaClient, review_marker
from .diff import ChangedFile, DiffError, get_changed_files, get_target_branch


class ClangFormatError(RuntimeError):
    """Raised when clang-format cannot perform a reliable format check."""


def clang_format_command() -> str:
    """Allow the common toolchain policy to select an exact binary."""
    return os.environ.get("CPP_CI_CLANG_FORMAT", "clang-format")


def validate_clang_format_config() -> None:
    try:
        result = subprocess.run(
            [clang_format_command(), "--dump-config"],
            capture_output=True,
            text=True,
        )
    except OSError as error:
        raise ClangFormatError(f"failed to execute {clang_format_command()}: {error}") from error

    if result.returncode != 0:
        details = result.stderr.strip() or result.stdout.strip() or "no diagnostic output"
        raise ClangFormatError(
            "clang-format configuration validation failed "
            f"(exit code {result.returncode}):\n{details}"
        )


def publish_inline_review(
        client: GiteaClient,
        issues: list[dict],
        *,
        dry_run: bool,
        verbose: bool,
) -> None:
    marker = review_marker("clang-format")
    pr_number = GiteaClient.resolve_pr_number()
    sha = GiteaClient.resolve_sha() or ""

    if pr_number is None:
        if verbose:
            print("[gitea] not a pull_request event — skipping review comments")
        if sha and not dry_run:
            state = "failure" if issues else "success"
            desc = (
                f"{len(issues)} formatting issue(s)"
                if issues
                else "clang-format clean"
            )
            try:
                client.publish_check(sha, state, "clang-format", desc)
            except Exception:
                pass
        return

    if dry_run:
        print(f"[dry-run] would dismiss previous reviews on PR #{pr_number}")
        for issue in issues:
            loc = issue["location"]
            print(
                f"[dry-run] would comment {loc['path']}:{loc['lines']['begin']}"
            )
        return

    try:
        client.dismiss_previous_reviews(pr_number, marker=marker)
    except Exception as e:
        print(f"[gitea] failed to clear previous reviews: {e}", file=sys.stderr)

    if issues:
        for issue in issues:
            loc = issue["location"]
            path = loc["path"]
            line = loc["lines"]["begin"]
            body = (f"**clang-format** violation on modified line(s) in this file.")
            client.add_review_comment(path, body, new_position=line)

        try:
            client.create_review(
                pr_number,
                body=f"clang-format found {len(issues)} issue(s).",
                event="COMMENT",
                commit_id=sha,
                marker=marker,
            )
        except Exception as e:
            print(f"[gitea] failed to create review: {e}", file=sys.stderr)

    if sha:
        state = "failure" if issues else "success"
        desc = (
            f"{len(issues)} formatting issue(s)"
            if issues
            else "clang-format clean"
        )
        try:
            client.publish_check(sha, state, "clang-format", desc)
        except Exception as e:
            print(f"[gitea] failed to publish check: {e}", file=sys.stderr)


def check_file(file: ChangedFile, fix: bool, verbose: bool) -> tuple[bool, str, str]:
    path = file.path

    try:
        with open(path) as f:
            original = f.read()
    except FileNotFoundError:
        return True, "", ""

    cmd = [clang_format_command(), path]

    for begin, end in file.ranges:
        cmd.append(f"--lines={begin}:{end}")

    try:
        result = subprocess.run(cmd, capture_output=True, text=True)
    except OSError as error:
        raise ClangFormatError(f"failed to execute clang-format for {path}: {error}") from error

    if result.returncode != 0:
        details = result.stderr.strip() or result.stdout.strip() or "no diagnostic output"
        raise ClangFormatError(
            f"clang-format failed for {path} (exit code {result.returncode}):\n{details}"
        )

    formatted = result.stdout

    compliant = formatted == original

    if not compliant and fix:
        with open(path, "w") as f:
            f.write(formatted)
        compliant = True

    return compliant, formatted, original


def main(*, excluded_paths: tuple[str, ...] = ()) -> None:
    parser = argparse.ArgumentParser(
        description=__doc__,
        formatter_class=argparse.RawDescriptionHelpFormatter,
    )
    parser.add_argument(
        "--base",
        help="branch/ref to diff against (default: CI var or 'main')",
    )
    parser.add_argument(
        "--files",
        nargs="+",
        help="explicit list of files to check, skips git diff",
    )
    parser.add_argument(
        "--exclude",
        action="append",
        default=[],
        help="repository-relative path prefix to exclude; can be repeated",
    )
    parser.add_argument(
        "--extension",
        action="append",
        default=[],
        help="C/C++ extension to analyse; can be repeated",
    )
    parser.add_argument(
        "--fix",
        action="store_true",
        help="auto-apply clang-format instead of just reporting",
    )
    parser.add_argument(
        "--dry-run",
        action="store_true",
        help="print results but don't write report, post reviews, or exit 1",
    )
    parser.add_argument(
        "--verbose",
        "-v",
        action="store_true",
        help="print debug info",
    )
    parser.add_argument(
        "--report-path",
        default="",
        help="output path for codequality report (kept for CI artifacts)",
    )
    parser.add_argument(
        "--no-gitea",
        action="store_true",
        help="skip Gitea review / check API calls",
    )
    args = parser.parse_args()

    try:
        validate_clang_format_config()
    except ClangFormatError as error:
        print(error, file=sys.stderr)
        sys.exit(2)

    base_ref = get_target_branch(args.base)
    try:
        changed = get_changed_files(
            base_ref,
            args.files,
            args.verbose,
            excluded_paths=(*excluded_paths, *args.exclude),
            extensions=args.extension or None,
        )
    except DiffError as error:
        print(f"[error] {error}", file=sys.stderr)
        sys.exit(2)

    issues = []
    try:
        for file in changed:
            compliant, formatted, original = check_file(file, args.fix, args.verbose)

            if not compliant:
                last_line = len(original.splitlines()) or 1
                fp = hashlib.md5(file.path.encode()).hexdigest()

                issues.append(
                    {
                        "description": f"Formatting issue in modified lines: {file.path}",
                        "check_name": "clang-format",
                        "fingerprint": fp,
                        "severity": "major",
                        "location": {
                            "path": file.path,
                            "lines": {"begin": last_line},
                        },
                    }
                )
    except ClangFormatError as error:
        print(error, file=sys.stderr)
        sys.exit(2)

    if not args.dry_run and len(args.report_path) > 0:
        with open(args.report_path, "w") as out:
            json.dump(issues, out, indent=2)

    if not args.no_gitea:
        client = GiteaClient.from_env(verbose=args.verbose)
        if client is not None:
            publish_inline_review(
                client,
                issues,
                dry_run=args.dry_run,
                verbose=args.verbose,
            )
        elif args.verbose:
            print("[gitea] client not configured — skipping review publish")

    if issues:
        print(f"Found {len(issues)} formatting issue(s):")
        for i in issues:
            print(f"  - {i['location']['path']}:{i['location']['lines']['begin']}")
        if not args.dry_run:
            sys.exit(1)
    else:
        print("All changed files are clang-format compliant.")


if __name__ == "__main__":
    main()
