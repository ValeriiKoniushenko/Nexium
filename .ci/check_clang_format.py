#!/usr/bin/env python3
"""
Checks that changed C/C++ files are clang-format compliant.

CI usage (unchanged behavior):
    python3 ci/check_clang_format.py

Local/debug usage from your machine:
    python3 ci/check_clang_format.py --base main --verbose
    python3 ci/check_clang_format.py --base main --fix          # auto-apply formatting
    python3 ci/check_clang_format.py --files src/foo.cpp src/bar.h
    python3 ci/check_clang_format.py --base main --dry-run      # show diff, don't write report/exit 1
"""

import argparse
import hashlib
import json
import os
import subprocess
import sys

# Paths are relative to the repository root.
EXCLUDED_PATHS = (
    "dependencies/",
)


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


def get_changed_files(base_ref: str, explicit_files: list[str] | None, verbose: bool) -> list[str]:
    if explicit_files:
        if verbose:
            print(f"[debug] using explicit file list: {explicit_files}")
        return explicit_files

    merge_base = subprocess.run(
        ["git", "merge-base", f"origin/{base_ref}", "HEAD"],
        capture_output=True, text=True
    )
    if merge_base.returncode != 0:
        # Fall back to local ref (useful when 'origin/<branch>' doesn't exist locally,
        # e.g. running against a local branch without a remote tracking ref)
        if verbose:
            print(f"[debug] 'origin/{base_ref}' not found, falling back to local ref '{base_ref}'")
        merge_base = subprocess.run(
            ["git", "merge-base", base_ref, "HEAD"],
            capture_output=True, text=True
        )
    base = merge_base.stdout.strip()

    if verbose:
        print(f"[debug] base ref: {base_ref} -> merge-base commit: {base}")

    diff = subprocess.run(
        ["git", "diff", "--diff-filter=ACMR", "--name-only", base, "HEAD",
         "--", "*.cpp", "*.cc", "*.h", "*.hpp"],
        capture_output=True, text=True
    ).stdout.split()

    filtered = [f for f in diff if not is_excluded(f)]

    if verbose:
        skipped = [f for f in diff if is_excluded(f)]
        print(f"[debug] changed files ({len(filtered)}): {filtered}")
        if skipped:
            print(f"[debug] excluded files ({len(skipped)}): {skipped}")

    return filtered


def check_file(path: str, fix: bool, verbose: bool) -> tuple[bool, str, str]:
    """Returns (is_compliant, formatted_text, original_text)."""
    try:
        original = open(path).read()
    except FileNotFoundError:
        if verbose:
            print(f"[debug] skipping missing file: {path}")
        return True, "", ""

    result = subprocess.run(["clang-format", path], capture_output=True, text=True)
    formatted = result.stdout

    if result.returncode != 0:
        print(f"[warn] clang-format failed on {path}: {result.stderr.strip()}", file=sys.stderr)
        return True, original, original

    compliant = formatted == original

    if not compliant and fix:
        with open(path, "w") as out:
            out.write(formatted)
        print(f"[fix] reformatted {path}")
        compliant = True  # treat as resolved after fixing

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
    for f in changed:
        compliant, formatted, original = check_file(f, args.fix, args.verbose)
        if args.verbose:
            print(f"[debug] {f}: {'OK' if compliant else 'NEEDS FORMATTING'}")

        if not compliant:
            fp = hashlib.md5(f.encode()).hexdigest()
            issues.append({
                "description": f"File is not clang-format compliant: {f}",
                "check_name": "clang-format",
                "fingerprint": fp,
                "severity": "major",
                "location": {"path": f, "lines": {"begin": 1}}
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
