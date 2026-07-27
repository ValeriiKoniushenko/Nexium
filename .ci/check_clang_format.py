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
import os
import subprocess
import sys
import re
from dataclasses import dataclass

# Paths are relative to the repository root.
EXCLUDED_PATHS = (
    "dependencies/",
    "docs/",
    "cmake/",
    "data/",
)
HUNK_RE = re.compile(r"^@@ -\d+(?:,\d+)? \+(\d+)(?:,(\d+))? @@")


@dataclass
class ChangedFile:
    path: str
    ranges: list[tuple[int, int]]

def is_excluded(path: str) -> bool:
    """Returns True if the file should be skipped."""
    normalized = path.replace("\\", "/")
    return any(normalized.startswith(prefix) for prefix in EXCLUDED_PATHS)


def get_target_branch(cli_base: str | None) -> str:
    """Resolve the branch/ref to diff against.

    Priority: --base flag > CI_MERGE_REQUEST_TARGET_BRANCH_NAME (GitLab CI) > 'main'.
    """
    if cli_base:
        return cli_base
    env_branch = os.environ.get("CI_MERGE_REQUEST_TARGET_BRANCH_NAME")
    if env_branch:
        return env_branch
    return "main"


def get_changed_files(base_ref: str, explicit_files: list[str] | None, verbose: bool) -> list[ChangedFile]:
    if explicit_files:
        return [ChangedFile(f, []) for f in explicit_files]

    merge_base = subprocess.run(
        ["git", "merge-base", f"origin/{base_ref}", "HEAD"],
        capture_output=True,
        text=True,
    )

    if merge_base.returncode != 0:
        merge_base = subprocess.run(
            ["git", "merge-base", base_ref, "HEAD"],
            capture_output=True,
            text=True,
        )

    base = merge_base.stdout.strip()

    diff = subprocess.run(
        [
            "git",
            "diff",
            "--diff-filter=ACMR",
            "-U0",
            base,
            "HEAD",
            "--",
            "*.cpp",
            "*.cc",
            "*.h",
            "*.hpp",
        ],
        capture_output=True,
        text=True,
    ).stdout.splitlines()

    files: list[ChangedFile] = []
    current = None

    for line in diff:
        if line.startswith("+++ b/"):
            path = line[6:]

            if is_excluded(path):
                current = None
                continue

            current = ChangedFile(path, [])
            files.append(current)
            continue

        if current is None:
            continue

        m = HUNK_RE.match(line)
        if not m:
            continue

        start = int(m.group(1))
        count = int(m.group(2) or "1")

        # deleted-only hunk
        if count == 0:
            continue

        current.ranges.append((start, start + count - 1))

    if verbose:
        print("[debug] changed files:")
        for f in files:
            print(f"  {f.path}: {f.ranges}")

    return files


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
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--base", help="branch/ref to diff against (default: CI var or 'main')")
    parser.add_argument("--files", nargs="+", help="explicit list of files to check, skips git diff")
    parser.add_argument("--fix", action="store_true", help="auto-apply clang-format instead of just reporting")
    parser.add_argument("--dry-run", action="store_true", help="print results but don't write report or exit 1")
    parser.add_argument("--verbose", "-v", action="store_true", help="print debug info")
    parser.add_argument("--report-path", default="gl-code-quality-report.json",
                        help="output path for codequality report")
    args = parser.parse_args()

    base_ref = get_target_branch(args.base)
    changed = get_changed_files(base_ref, args.files, args.verbose)

    issues = []
    for file in changed:
        compliant, formatted, original = check_file(file, args.fix, args.verbose)

        if not compliant:
            fp = hashlib.md5(file.path.encode()).hexdigest()

            issues.append({
                "description": f"Formatting issue in modified lines: {file.path}",
                "check_name": "clang-format",
                "fingerprint": fp,
                "severity": "major",
                "location": {
                    "path": file.path,
                    "lines": {
                        "begin": file.ranges[0][0]
                    }
                }
            })

    with open(args.report_path, "w") as out:
        json.dump(issues, out, indent=2)

    if issues:
        print(f"Found {len(issues)} formatting issue(s):")
        for i in issues:
            print(f"  - {i['location']['path']}")
        if not args.dry_run:
            sys.exit(1)
    else:
        print("All changed files are clang-format compliant.")


if __name__ == "__main__":
    main()
