"""Reliable changed-C/C++ discovery shared by format and tidy checks."""

from __future__ import annotations

import os
import re
import subprocess
from dataclasses import dataclass
from typing import Iterable


HUNK_RE = re.compile(r"^@@ -\d+(?:,\d+)? \+(\d+)(?:,(\d+))? @@")
CPP_HEADER_SUFFIXES = {".h", ".hh", ".hpp", ".hxx"}
CPP_SOURCE_SUFFIXES = (".c", ".cc", ".cpp", ".cxx")
DEFAULT_CPP_SUFFIXES = frozenset((*CPP_HEADER_SUFFIXES, *CPP_SOURCE_SUFFIXES))


class DiffError(RuntimeError):
    """The requested comparison cannot be trusted."""


@dataclass
class ChangedFile:
    path: str
    ranges: list[tuple[int, int]]


def normalize_repo_path(path: str) -> str:
    """Return a stable repository-relative path when possible."""
    absolute = os.path.abspath(path)
    repo_root = os.path.abspath(os.getcwd())
    try:
        if os.path.commonpath((repo_root, absolute)) == repo_root:
            return os.path.relpath(absolute, repo_root).replace("\\", "/")
    except ValueError:
        pass
    return os.path.normpath(path).replace("\\", "/")


def is_cpp_header(path: str) -> bool:
    return os.path.splitext(path)[1].lower() in CPP_HEADER_SUFFIXES


def get_cpp_analysis_targets(changed: list[ChangedFile]) -> list[str]:
    """Choose translation units, using a sibling source for changed headers."""
    targets: list[str] = []
    seen: set[str] = set()
    for changed_file in changed:
        target = changed_file.path
        if is_cpp_header(target):
            root, _ = os.path.splitext(target)
            for suffix in CPP_SOURCE_SUFFIXES:
                sibling = root + suffix
                if os.path.isfile(sibling):
                    target = sibling
                    break
        target = normalize_repo_path(target)
        if target not in seen:
            seen.add(target)
            targets.append(target)
    return targets


def is_excluded(path: str, excluded_paths: tuple[str, ...]) -> bool:
    normalized = path.replace("\\", "/")
    return any(normalized.startswith(prefix) for prefix in excluded_paths)


def get_target_branch(cli_base: str | None) -> str:
    if cli_base:
        return cli_base
    for key in ("GITEA_BASE_REF", "GITHUB_BASE_REF", "CI_MERGE_REQUEST_TARGET_BRANCH_NAME"):
        value = os.environ.get(key)
        if value:
            return value
    return "main"


def prepare_base_ref(base_ref: str, *, verbose: bool = False) -> str:
    """Fetch and verify the exact base ref before a CI diff.

    A missing merge base used to silently produce an empty analysis result.  A
    check now fails instead, which is much safer for a required quality gate.
    """
    remote_ref = f"refs/remotes/origin/{base_ref}"
    fetch = subprocess.run(
        ["git", "fetch", "--no-tags", "origin", f"+refs/heads/{base_ref}:{remote_ref}"],
        capture_output=True,
        text=True,
    )
    if fetch.returncode != 0:
        details = (fetch.stderr or fetch.stdout).strip() or "no diagnostic output"
        raise DiffError(f"cannot fetch base branch {base_ref!r}: {details}")
    verify = subprocess.run(["git", "rev-parse", "--verify", remote_ref], capture_output=True, text=True)
    if verify.returncode != 0:
        raise DiffError(f"base branch {base_ref!r} was not available after fetch")
    resolved = verify.stdout.strip()
    if verbose:
        print(f"[diff] base {base_ref} resolved to {resolved}")
    return resolved


def is_changed_line(file: ChangedFile, line: int) -> bool:
    return any(begin <= line <= end for begin, end in file.ranges)


def _merge_base(base_ref: str) -> str:
    candidates = (f"origin/{base_ref}", base_ref)
    diagnostics: list[str] = []
    for candidate in candidates:
        result = subprocess.run(["git", "merge-base", candidate, "HEAD"], capture_output=True, text=True)
        if result.returncode == 0 and result.stdout.strip():
            return result.stdout.strip()
        diagnostics.append((result.stderr or result.stdout).strip() or f"exit code {result.returncode}")
    raise DiffError(
        f"cannot find a merge base for {base_ref!r} and HEAD; "
        "fetch the base branch or pass --files explicitly. "
        f"Diagnostics: {'; '.join(diagnostics)}"
    )


def _normalise_extensions(extensions: Iterable[str] | None) -> frozenset[str]:
    if extensions is None:
        return DEFAULT_CPP_SUFFIXES
    normalized = frozenset(
        value.lower() if value.startswith(".") else f".{value.lower()}" for value in extensions
    )
    if not normalized:
        raise DiffError("the C/C++ extension list must not be empty")
    return normalized


def get_changed_files(
    base_ref: str,
    explicit_files: list[str] | None,
    verbose: bool,
    *,
    excluded_paths: tuple[str, ...] = (),
    extensions: Iterable[str] | None = None,
) -> list[ChangedFile]:
    """Return changed files and added-line ranges, or fail rather than skipping work."""
    suffixes = _normalise_extensions(extensions)
    if explicit_files:
        return [
            ChangedFile(normalize_repo_path(path), [])
            for path in explicit_files
            if os.path.splitext(path)[1].lower() in suffixes and not is_excluded(path, excluded_paths)
        ]

    base = _merge_base(base_ref)
    result = subprocess.run(
        ["git", "diff", "--diff-filter=ACMR", "-U0", base, "HEAD", "--"],
        capture_output=True,
        text=True,
    )
    if result.returncode != 0:
        details = (result.stderr or result.stdout).strip() or "no diagnostic output"
        raise DiffError(f"git diff against {base_ref!r} failed: {details}")

    files: list[ChangedFile] = []
    current: ChangedFile | None = None
    for line in result.stdout.splitlines():
        if line.startswith("+++ b/"):
            path = line[6:]
            if is_excluded(path, excluded_paths) or os.path.splitext(path)[1].lower() not in suffixes:
                current = None
                continue
            current = ChangedFile(path, [])
            files.append(current)
            continue
        if current is None:
            continue
        match = HUNK_RE.match(line)
        if not match:
            continue
        start = int(match.group(1))
        count = int(match.group(2) or "1")
        if count:
            current.ranges.append((start, start + count - 1))

    if verbose:
        print("[diff] changed files:")
        for changed_file in files:
            print(f"  {changed_file.path}: {changed_file.ranges}")
    return files
