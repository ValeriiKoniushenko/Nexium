#!/usr/bin/env python3
"""
Checks that changed C/C++ files are clang-format compliant.

CI usage (unchanged behavior):
    python3 .ci/check_clang_format.py

Local/debug usage from your machine:
    python3 .ci/check_clang_format.py --base develop --verbose
    python3 .ci/check_clang_format.py --base develop --fix          # auto-apply formatting
    python3 .ci/check_clang_format.py --files src/foo.cpp src/bar.h
    python3 .ci/check_clang_format.py --base develop --dry-run      # show diff, don't write report/exit 1
"""

import argparse
import hashlib
import json
import subprocess
import sys

from gitea_client import GiteaClient
from utils import ChangedFile, get_changed_files, get_target_branch, is_changed_line


def first_mismatch_line(original: str, formatted: str, file: ChangedFile) -> int:
    """Return the best line number for an inline comment on a formatting diff."""
    orig_lines = original.splitlines()
    fmt_lines = formatted.splitlines()

    candidates: list[int] = []
    for i, (a, b) in enumerate(zip(orig_lines, fmt_lines), start=1):
        if a != b:
            candidates.append(i)

    if len(orig_lines) != len(fmt_lines):
        candidates.append(min(len(orig_lines), len(fmt_lines)) + 1)

    for line in candidates:
        if not file.ranges or is_changed_line(file, line):
            return line

    if file.ranges:
        return file.ranges[0][0]
    return candidates[0] if candidates else 1


def publish_inline_review(
    client: GiteaClient,
    issues: list[dict],
    *,
    dry_run: bool,
    verbose: bool,
) -> None:
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
        client.dismiss_previous_reviews(pr_number)
    except Exception as e:
        print(f"[gitea] failed to clear previous reviews: {e}", file=sys.stderr)

    if issues:
        for issue in issues:
            loc = issue["location"]
            path = loc["path"]
            line = loc["lines"]["begin"]
            body = (
                f"**clang-format** violation on modified lines\n\n"
                f"{issue['description']}\n\n"
                f"Run `clang-format -i {path}` (or pass `--fix`) to apply."
            )
            client.add_review_comment(path, body, new_position=line)

        try:
            client.create_review(
                pr_number,
                body=f"clang-format found {len(issues)} issue(s).",
                event="COMMENT",
                commit_id=sha,
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

    cmd = ["clang-format", path]

    for begin, end in file.ranges:
        cmd.append(f"--lines={begin}:{end}")

    result = subprocess.run(cmd, capture_output=True, text=True)

    if result.returncode != 0:
        print(result.stderr, file=sys.stderr)
        return True, original, original

    formatted = result.stdout

    compliant = formatted == original

    if not compliant and fix:
        with open(path, "w") as f:
            f.write(formatted)
        compliant = True

    return compliant, formatted, original


def main():
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
        default="gl-code-quality-report.json",
        help="output path for codequality report (kept for CI artifacts)",
    )
    parser.add_argument(
        "--no-gitea",
        action="store_true",
        help="skip Gitea review / check API calls",
    )
    args = parser.parse_args()

    base_ref = get_target_branch(args.base)
    changed = get_changed_files(base_ref, args.files, args.verbose)

    issues = []
    for file in changed:
        compliant, formatted, original = check_file(file, args.fix, args.verbose)

        if not compliant:
            line = first_mismatch_line(original, formatted, file)
            fp = hashlib.md5(file.path.encode()).hexdigest()

            issues.append(
                {
                    "description": f"Formatting issue in modified lines: {file.path}",
                    "check_name": "clang-format",
                    "fingerprint": fp,
                    "severity": "major",
                    "location": {
                        "path": file.path,
                        "lines": {"begin": line},
                    },
                }
            )

    if not args.dry_run:
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
