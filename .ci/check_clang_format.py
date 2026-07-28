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
import sys
import os
import subprocess
import urllib.request
import urllib.error
from utils import get_target_branch, get_changed_files, ChangedFile

def publish_gitea_comment(issue: dict, verbose: bool):
    token = os.environ.get("GITEA_TOKEN")

    if not token:
        if verbose:
            print("GITEA_TOKEN not set, skipping PR comment")
        return

    repo = os.environ.get("GITEA_REPOSITORY")
    sha = os.environ.get("GITEA_SHA")
    server = os.environ.get("GITEA_SERVER_URL")

    if not all([repo, sha, server]):
        if verbose:
            print("Missing Gitea environment variables, skipping PR comment")
        return

    # Find PR number from Gitea event payload
    event_path = os.environ.get("GITEA_EVENT_PATH")
    if not event_path:
        return

    with open(event_path) as f:
        event = json.load(f)

    pr_number = event.get("number")
    if not pr_number:
        if verbose:
            print("Not running in pull request context")
        return

    owner, name = repo.split("/", 1)

    url = (
        f"{server}/api/v1/repos/{owner}/{name}"
        f"/issues/{pr_number}/comments"
    )

    location = issue["location"]

    body = (
        f"❌ **clang-format violation**\n\n"
        f"File: `{location['path']}`\n"
        f"Line: `{location['lines']['begin']}`\n\n"
        f"{issue['description']}"
    )

    payload = json.dumps({
        "body": body
    }).encode()

    request = urllib.request.Request(
        url,
        data=payload,
        headers={
            "Authorization": f"token {token}",
            "Content-Type": "application/json",
        },
        method="POST",
    )

    try:
        with urllib.request.urlopen(request) as response:
            if verbose:
                print(
                    f"Created Gitea comment: {response.status}"
                )

    except urllib.error.HTTPError as e:
        print(
            f"Failed to create Gitea comment: {e.code} {e.reason}",
            file=sys.stderr,
        )

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

    for issue in issues:
        publish_gitea_comment(issue, args.verbose)
        
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
