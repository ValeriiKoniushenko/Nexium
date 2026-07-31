#!/usr/bin/env python3

import os
import subprocess
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

    Priority:
      --base flag
      > GITEA_BASE_REF / GITHUB_BASE_REF (Gitea Actions pull_request)
      > CI_MERGE_REQUEST_TARGET_BRANCH_NAME (GitLab CI)
      > 'main'
    """
    if cli_base:
        return cli_base
    for key in (
        "GITEA_BASE_REF",
        "GITHUB_BASE_REF",
        "CI_MERGE_REQUEST_TARGET_BRANCH_NAME",
    ):
        env_branch = os.environ.get(key)
        if env_branch:
            return env_branch
    return "main"

def is_changed_line(file: ChangedFile, line: int) -> bool:
    for begin, end in file.ranges:
        if begin <= line <= end:
            return True
    return False

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

