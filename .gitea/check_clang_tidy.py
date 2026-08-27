#!/usr/bin/env python3
"""
Runs clang-tidy over changed C/C++ files and reports issues in
GitLab code-quality JSON format.

CI usage (unchanged behavior):
    python3 .gitea/check_clang_tidy.py

Local/debug usage from your machine:
    python3 .gitea/check_clang_tidy.py --base develop --verbose
    python3 .gitea/check_clang_tidy.py --files src/foo.cpp --build-dir build --verbose
    python3 .gitea/check_clang_tidy.py --base develop --fail-on error   # only fail on 'error' level
    python3 .gitea/check_clang_tidy.py --base develop --dry-run         # don't write report / exit 1
"""
import argparse
import concurrent.futures
import hashlib
import json
import os
import re
import subprocess
import sys
from dataclasses import dataclass
from gitea_client import GiteaClient, review_marker
from utils import (
    ChangedFile,
    get_changed_files,
    get_cpp_analysis_targets,
    get_target_branch,
    is_changed_line,
    is_cpp_header,
    normalize_repo_path,
)

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

DEFAULT_MAX_JOBS = 4


@dataclass
class TidyResult:
    path: str
    command: list[str]
    returncode: int
    stdout: str
    stderr: str


def publish_inline_review(
    client: GiteaClient,
    issues: list[dict],
    *,
    failed: bool,
    dry_run: bool,
    verbose: bool,
) -> None:
    """Replace this check's previous review with the current diagnostics."""
    pr_number = GiteaClient.resolve_pr_number()
    sha = GiteaClient.resolve_sha() or ""
    marker = review_marker("clang-tidy")

    if pr_number is None:
        if verbose:
            print("[gitea] not a pull_request event — skipping review comments")
        if sha and not dry_run:
            state = "failure" if failed else "success"
            desc = f"{len(issues)} clang-tidy issue(s)" if issues else "clang-tidy clean"
            try:
                client.publish_check(sha, state, "clang-tidy", desc)
            except Exception as e:
                print(f"[gitea] failed to publish check: {e}", file=sys.stderr)
        return

    if dry_run:
        print(f"[dry-run] would dismiss previous clang-tidy review on PR #{pr_number}")
        for issue in issues:
            loc = issue["location"]
            print(f"[dry-run] would comment {loc['path']}:{loc['lines']['begin']}")
        return

    try:
        client.dismiss_previous_reviews(pr_number, marker=marker)
    except Exception as e:
        print(f"[gitea] failed to clear previous clang-tidy reviews: {e}", file=sys.stderr)

    if issues:
        for issue in issues:
            loc = issue["location"]
            client.add_review_comment(
                loc["path"],
                f"**clang-tidy** `{issue['check_name']}`\n\n{issue['description']}",
                new_position=loc["lines"]["begin"],
            )

        try:
            client.create_review(
                pr_number,
                body=f"clang-tidy found {len(issues)} issue(s) on modified lines.",
                event="COMMENT",
                commit_id=sha,
                marker=marker,
            )
        except Exception as e:
            print(f"[gitea] failed to create review: {e}", file=sys.stderr)

    if sha:
        state = "failure" if failed else "success"
        desc = f"{len(issues)} clang-tidy issue(s)" if issues else "clang-tidy clean"
        try:
            client.publish_check(sha, state, "clang-tidy", desc)
        except Exception as e:
            print(f"[gitea] failed to publish check: {e}", file=sys.stderr)


def make_changed_header_filter(changed: list[ChangedFile]) -> str:
    """Restrict header diagnostics to headers that are part of the diff."""
    patterns = []
    for changed_file in changed:
        if not is_cpp_header(changed_file.path):
            continue

        path = re.escape(normalize_repo_path(changed_file.path))
        patterns.append(rf"(?:^|.*/){path}$")

    return "|".join(patterns)


def run_clang_tidy(
    path: str,
    build_dir: str,
    header_filter: str,
    extra_args: list[str],
) -> TidyResult:
    # NOTE: clang-tidy has no "-v" flag (that's a run-clang-tidy-ism) and the
    # target file must be a plain positional arg, not "-f <file>".
    cmd = [
        "clang-tidy",
        f"--header-filter={header_filter}",
        "-p",
        build_dir,
        *extra_args,
        path,
    ]

    result = subprocess.run(cmd, capture_output=True, text=True)
    return TidyResult(path, cmd, result.returncode, result.stdout, result.stderr)


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--base", help="branch/ref to diff against (default: CI var or 'main')")
    parser.add_argument("--files", nargs="+", help="explicit list of files to check, skips git diff")
    parser.add_argument("--build-dir", default="build", help="compile_commands.json directory (default: build)")
    parser.add_argument("--header-filter",
                        help="clang-tidy --header-filter value (default: changed headers only)")
    parser.add_argument("--jobs", "-j", type=int, default=0,
                        help=f"parallel clang-tidy processes (default: auto, capped at {DEFAULT_MAX_JOBS})")
    parser.add_argument("--fail-on", choices=["error", "warning", "note"], default="warning",
                        help="minimum level that causes non-zero exit (default: warning)")
    parser.add_argument("--extra-arg", action="append", default=[],
                        help="extra raw args passed to clang-tidy, can repeat, e.g. --extra-arg=-checks=-*,bugprone-*")
    parser.add_argument("--dry-run", action="store_true", help="print results but don't write report or exit 1")
    parser.add_argument("--verbose", "-v", action="store_true", help="print debug info incl. raw clang-tidy output")
    parser.add_argument("--report-path", default="gl-code-quality-report.json",
                        help="output path for codequality report")
    parser.add_argument("--no-gitea", action="store_true",
                        help="skip Gitea review / check API calls")
    args = parser.parse_args()

    if not os.path.isdir(args.build_dir):
        print(f"[error] build dir '{args.build_dir}' not found. Run cmake configure first, or pass --build-dir.", file=sys.stderr)
        sys.exit(2)

    if args.jobs < 0:
        parser.error("--jobs must be zero (auto) or a positive integer")

    base_ref = get_target_branch(args.base)
    changed = get_changed_files(base_ref, args.files, args.verbose)
    analysis_targets = get_cpp_analysis_targets(changed)
    header_filter = args.header_filter
    if header_filter is None:
        header_filter = make_changed_header_filter(changed)

    available_jobs = args.jobs or min(DEFAULT_MAX_JOBS, os.cpu_count() or 1)
    job_count = min(available_jobs, len(analysis_targets)) if analysis_targets else 0

    if args.verbose:
        print(f"[debug] analysis targets ({job_count} parallel job(s)):")
        for target in analysis_targets:
            print(f"  {target}")
        print(f"[debug] header filter: {header_filter!r}")

    # order: error > warning > note, used to decide whether to fail
    level_rank = {"note": 0, "warning": 1, "error": 2}
    fail_threshold = level_rank[args.fail_on]

    issues = []
    seen_fingerprints = set()
    should_fail = False

    existing_targets = []
    for target in analysis_targets:
        if os.path.isfile(target):
            existing_targets.append(target)
        elif args.verbose:
            print(f"[debug] skipping missing file: {target}")

    if existing_targets:
        with concurrent.futures.ThreadPoolExecutor(max_workers=job_count) as executor:
            futures = [
                executor.submit(
                    run_clang_tidy,
                    target,
                    args.build_dir,
                    header_filter,
                    args.extra_arg,
                )
                for target in existing_targets
            ]
            tidy_results = [future.result() for future in futures]
    else:
        tidy_results = []

    changed_by_path = {
        normalize_repo_path(changed_file.path): changed_file
        for changed_file in changed
    }

    for result in tidy_results:
        if args.verbose:
            print(f"[debug] running: {' '.join(result.command)}")
            print(f"[debug] clang-tidy exit code: {result.returncode} for {result.path}")
            if result.returncode != 0 and result.stderr:
                print(f"[debug] stderr: {result.stderr[:2000]}")
            print(f"[debug] --- raw clang-tidy output for {result.path} ---")
            print(result.stdout)
            print("[debug] --- end output ---")

        if result.returncode != 0 and not result.stdout.strip():
            print(
                f"[error] clang-tidy produced no output and exited "
                f"{result.returncode} for {result.path}",
                file=sys.stderr,
            )
            if result.stderr:
                print(result.stderr, file=sys.stderr)

        for line in result.stdout.splitlines():
            m = DIAG_RE.match(line)
            if not m or m.group("level") == "note":
                continue

            line_no = int(m.group("line"))
            diagnostic_path = normalize_repo_path(m.group("file"))
            changed_file = changed_by_path.get(diagnostic_path)

            if changed_file is None or not is_changed_line(changed_file, line_no):
                continue

            check = m.group("check") or "clang-tidy"

            fp_src = f"{diagnostic_path}:{line_no}:{m.group('col')}:{check}"
            fp = hashlib.md5(fp_src.encode()).hexdigest()
            if fp in seen_fingerprints:
                continue
            seen_fingerprints.add(fp)

            is_error = ""
            if level_rank[m.group("level")] >= fail_threshold and check != "clang-diagnostic-error":
                is_error = "❗️**ERROR:**"
                should_fail = True

            issues.append({
                "description": f"{is_error} {m.group('message')}",
                "check_name": check,
                "fingerprint": fp,
                "severity": SEVERITY_MAP.get(m.group("level"), "major"),
                "location": {
                    "path": diagnostic_path,
                    "lines": {"begin": line_no}
                }
            })

    if not args.dry_run and args.report_path:
        with open(args.report_path, "w") as out:
            json.dump(issues, out, indent=2)

    if not args.no_gitea:
        client = GiteaClient.from_env(verbose=args.verbose)
        if client is not None:
            publish_inline_review(
                client,
                issues,
                failed=should_fail,
                dry_run=args.dry_run,
                verbose=args.verbose,
            )
        elif args.verbose:
            print("[gitea] client not configured — skipping review publish")

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
