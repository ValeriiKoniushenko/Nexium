#!/usr/bin/env python3
"""
Runs clang-tidy over changed C/C++ files and reports issues in
GitLab code-quality JSON format.

CI usage (unchanged behavior):
    python3 ci/check_clang_tidy.py

Local/debug usage from your machine:
    python3 ci/check_clang_tidy.py --base main --verbose
    python3 ci/check_clang_tidy.py --files src/foo.cpp --build-dir build --verbose
    python3 ci/check_clang_tidy.py --base main --fail-on error   # only fail on 'error' level
    python3 ci/check_clang_tidy.py --base main --dry-run         # don't write report / exit 1
"""
import argparse
import hashlib
import json
import os
import re
import subprocess
import sys

SEVERITY_MAP = {
    "error": "critical",
    "warning": "major",
    "note": "minor",
}

# matches e.g.: src/foo.cpp:42:7: warning: message [check-name]
DIAG_RE = re.compile(
    r'^(?P<file>[^:]+):(?P<line>\d+):(?P<col>\d+): '
    r'(?P<level>error|warning|note): (?P<message>.*?)(?: \[(?P<check>[\w,.\-]+)\])?$'
)


def get_target_branch(cli_base: str | None) -> str:
    if cli_base:
        return cli_base
    env_branch = os.environ.get("CI_MERGE_REQUEST_TARGET_BRANCH_NAME")
    print(env_branch)
    if env_branch:
        return env_branch
    return "develop"


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
         "--", "*.cpp", "*.cc"],
        capture_output=True, text=True
    ).stdout.split()

    if verbose:
        print(f"[debug] changed files ({len(diff)}): {diff}")

    return diff


def run_clang_tidy(f: str, build_dir: str, header_filter: str, extra_args: list[str], verbose: bool) -> str:
    # NOTE: clang-tidy has no "-v" flag (that's a run-clang-tidy-ism) and the
    # target file must be a plain positional arg, not "-f <file>".
    cmd = ["clang-tidy", f"--header-filter={header_filter}", "-p", build_dir, *extra_args, f]
    if verbose:
        print(f"[debug] running: {' '.join(cmd)}")

    result = subprocess.run(cmd, capture_output=True, text=True)

    if verbose:
        print(f"[debug] clang-tidy exit code: {result.returncode} for {f}")
        if result.returncode != 0 and result.stderr:
            print(f"[debug] stderr: {result.stderr[:2000]}")

    if result.returncode != 0 and not result.stdout.strip():
        print(f"[error] clang-tidy produced no output and exited {result.returncode} for {f}", file=sys.stderr)
        if result.stderr:
            print(result.stderr, file=sys.stderr)

    return result.stdout


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--base", help="branch/ref to diff against (default: CI var or 'main')")
    parser.add_argument("--files", nargs="+", help="explicit list of files to check, skips git diff")
    parser.add_argument("--build-dir", default="build", help="compile_commands.json directory (default: build)")
    parser.add_argument("--header-filter", default="",
                        help="clang-tidy --header-filter value (default: '' = don't report on headers, matches run-clang-tidy default)")
    parser.add_argument("--fail-on", choices=["error", "warning", "note"], default="warning",
                        help="minimum level that causes non-zero exit (default: warning)")
    parser.add_argument("--extra-arg", action="append", default=[],
                        help="extra raw args passed to clang-tidy, can repeat, e.g. --extra-arg=-checks=-*,bugprone-*")
    parser.add_argument("--dry-run", action="store_true", help="print results but don't write report or exit 1")
    parser.add_argument("--verbose", "-v", action="store_true", help="print debug info incl. raw clang-tidy output")
    parser.add_argument("--report-path", default="gl-code-quality-report.json",
                        help="output path for codequality report")
    args = parser.parse_args()

    if not os.path.isdir(args.build_dir):
        print(f"[error] build dir '{args.build_dir}' not found. "
              f"Run cmake configure first, or pass --build-dir.", file=sys.stderr)
        sys.exit(2)

    base_ref = get_target_branch(args.base)
    changed = get_changed_files(base_ref, args.files, args.verbose)

    # order: error > warning > note, used to decide whether to fail
    level_rank = {"note": 0, "warning": 1, "error": 2}
    fail_threshold = level_rank[args.fail_on]

    issues = []
    should_fail = False

    for f in changed:
        if not os.path.isfile(f):
            if args.verbose:
                print(f"[debug] skipping missing file: {f}")
            continue

        stdout = run_clang_tidy(f, args.build_dir, args.header_filter, args.extra_arg, args.verbose)

        if args.verbose:
            print(f"[debug] --- raw clang-tidy output for {f} ---")
            print(stdout)
            print("[debug] --- end output ---")

        for line in stdout.splitlines():
            m = DIAG_RE.match(line)
            if not m or m.group("level") == "note":
                continue

            check = m.group("check") or "clang-tidy"
            fp_src = f"{f}:{m.group('line')}:{m.group('col')}:{check}"
            fp = hashlib.md5(fp_src.encode()).hexdigest()

            issues.append({
                "description": f"[{check}] {m.group('message')}",
                "check_name": check,
                "fingerprint": fp,
                "severity": SEVERITY_MAP.get(m.group("level"), "major"),
                "location": {
                    "path": f,
                    "lines": {"begin": int(m.group("line"))}
                }
            })

            if level_rank[m.group("level")] >= fail_threshold:
                should_fail = True

    with open(args.report_path, "w") as out:
        json.dump(issues, out, indent=2)

    if issues:
        print(f"Found {len(issues)} clang-tidy issue(s):")
        for i in issues:
            print(f"  - {i['location']['path']}:{i['location']['lines']['begin']}: {i['description']}")
        if should_fail and not args.dry_run:
            sys.exit(1)
    else:
        print("No clang-tidy issues found.")


if __name__ == "__main__":
    main()
