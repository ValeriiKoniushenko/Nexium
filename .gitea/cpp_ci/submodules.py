"""Cache pinned Git submodule metadata without sharing a job worktree.

Each CI job still receives its own checkout.  The persistent cache contains
only the ``.git/modules`` metadata, which is copied into that checkout before
Git materializes its submodule worktrees.  This keeps concurrent jobs isolated
while allowing an unchanged pinned submodule graph to be restored without a
network fetch.
"""

from __future__ import annotations

import argparse
import contextlib
import fcntl
import hashlib
import json
import os
import shutil
import subprocess
import sys
import tempfile
from dataclasses import dataclass
from pathlib import Path


CACHE_FORMAT = 1


class SubmoduleCacheError(RuntimeError):
    """Raised when the persistent submodule cache cannot be used safely."""


@dataclass(frozen=True)
class CachePaths:
    """All persistent paths for one repository's submodule metadata."""

    root: Path
    modules: Path
    metadata: Path
    lock: Path


def _run_git(
        repository: Path,
        arguments: list[str],
        *,
        capture_output: bool = False,
) -> subprocess.CompletedProcess[bytes]:
    return subprocess.run(
        ["git", *arguments],
        cwd=repository,
        check=True,
        stdout=subprocess.PIPE if capture_output else None,
    )


def _git_text(repository: Path, arguments: list[str]) -> str:
    return _run_git(repository, arguments, capture_output=True).stdout.decode().strip()


def _cached_gitlink_entries(repository: Path) -> list[bytes]:
    output = _run_git(repository, ["ls-files", "--stage", "-z"], capture_output=True).stdout
    return [
        entry
        for entry in output.split(b"\0")
        if entry and entry.split(b"\t", maxsplit=1)[0].split(maxsplit=1)[0] == b"160000"
    ]


def submodule_signature(repository: Path) -> str | None:
    """Return the identity of the complete pinned submodule graph root.

    A nested submodule can change only when its containing direct submodule's
    gitlink changes, so the top-level gitlinks plus ``.gitmodules`` are enough
    to invalidate a recursive metadata cache.
    """
    entries = _cached_gitlink_entries(repository)
    if not entries:
        return None

    gitmodules = repository / ".gitmodules"
    if not gitmodules.is_file():
        raise SubmoduleCacheError("the checkout has submodule gitlinks but no .gitmodules file")

    digest = hashlib.sha256(b"cpp-ci-submodule-cache-v1\0")
    digest.update(gitmodules.read_bytes())
    for entry in entries:
        digest.update(b"\0")
        digest.update(entry)
    return digest.hexdigest()


def _cache_namespace(repository: Path, configured: str | None) -> str:
    if configured:
        return configured

    result = subprocess.run(
        ["git", "config", "--get", "remote.origin.url"],
        cwd=repository,
        stdout=subprocess.PIPE,
        stderr=subprocess.DEVNULL,
    )
    if result.returncode == 0 and result.stdout.strip():
        return result.stdout.decode().strip()
    return str(repository.resolve())


def cache_paths(cache_root: Path, namespace: str) -> CachePaths:
    """Return a traversal-safe, repository-scoped cache directory."""
    digest = hashlib.sha256(namespace.encode()).hexdigest()
    root = cache_root.resolve() / digest
    return CachePaths(
        root=root,
        modules=root / "modules",
        metadata=root / "metadata.json",
        lock=root / "lock",
    )


def _read_metadata(path: Path) -> str | None:
    try:
        data = json.loads(path.read_text(encoding="utf-8"))
    except (FileNotFoundError, json.JSONDecodeError, OSError):
        return None
    if not isinstance(data, dict) or data.get("format") != CACHE_FORMAT:
        return None
    signature = data.get("signature")
    return signature if isinstance(signature, str) else None


def _cache_is_current(paths: CachePaths, signature: str) -> bool:
    return paths.modules.is_dir() and _read_metadata(paths.metadata) == signature


@contextlib.contextmanager
def _locked(path: Path, *, exclusive: bool):
    path.parent.mkdir(parents=True, exist_ok=True)
    with path.open("a+", encoding="utf-8") as lock_file:
        mode = fcntl.LOCK_EX if exclusive else fcntl.LOCK_SH
        fcntl.flock(lock_file.fileno(), mode)
        try:
            yield
        finally:
            fcntl.flock(lock_file.fileno(), fcntl.LOCK_UN)


def _git_modules_directory(repository: Path) -> Path:
    git_dir = Path(_git_text(repository, ["rev-parse", "--absolute-git-dir"])).resolve()
    return git_dir / "modules"


def _remove_modules(path: Path) -> None:
    if not path.exists() and not path.is_symlink():
        return
    if path.is_symlink() or not path.is_dir():
        raise SubmoduleCacheError(f"refusing to remove unexpected Git modules path: {path}")
    shutil.rmtree(path)


def _copy_modules(source: Path, destination: Path) -> None:
    if not source.is_dir():
        raise SubmoduleCacheError(f"submodule metadata directory does not exist: {source}")
    destination.mkdir(parents=True, exist_ok=True)
    try:
        subprocess.run(
            [
                "cp",
                "-a",
                "--reflink=auto",
                "--no-preserve=ownership",
                f"{source}/.",
                str(destination),
            ],
            check=True,
        )
    except FileNotFoundError as error:
        raise SubmoduleCacheError("the CI image needs the GNU cp command for the submodule cache") from error


def _restore_modules(repository: Path, cached_modules: Path) -> bool:
    modules = _git_modules_directory(repository)
    _remove_modules(modules)
    if not cached_modules.is_dir():
        return False
    _copy_modules(cached_modules, modules)
    return True


def _update_submodules(repository: Path, *, no_fetch: bool) -> None:
    _run_git(repository, ["submodule", "sync", "--recursive"])
    command = ["submodule", "update", "--init", "--recursive", "--depth", "1"]
    if no_fetch:
        command.append("--no-fetch")
    _run_git(repository, command)


def _store_modules(repository: Path, paths: CachePaths, signature: str) -> None:
    source = _git_modules_directory(repository)
    if not source.is_dir():
        raise SubmoduleCacheError("submodule update completed without creating Git module metadata")

    paths.root.mkdir(parents=True, exist_ok=True)
    staging = Path(tempfile.mkdtemp(prefix=".staging-", dir=paths.root))
    try:
        staged_modules = staging / "modules"
        staged_metadata = staging / "metadata.json"
        _copy_modules(source, staged_modules)
        staged_metadata.write_text(
            json.dumps({"format": CACHE_FORMAT, "signature": signature}, sort_keys=True) + "\n",
            encoding="utf-8",
        )

        if paths.metadata.exists() or paths.metadata.is_symlink():
            if paths.metadata.is_symlink() or not paths.metadata.is_file():
                raise SubmoduleCacheError(f"refusing to replace unexpected cache metadata path: {paths.metadata}")
            paths.metadata.unlink()
        _remove_modules(paths.modules)
        staged_modules.replace(paths.modules)
        staged_metadata.replace(paths.metadata)
    finally:
        shutil.rmtree(staging, ignore_errors=True)


def _refresh_cache(repository: Path, paths: CachePaths, signature: str) -> None:
    _restore_modules(repository, paths.modules)
    _update_submodules(repository, no_fetch=False)
    _store_modules(repository, paths, signature)


def prepare(
        repository: Path,
        *,
        cache_root: Path,
        cache_namespace: str | None = None,
) -> bool:
    """Materialize pinned submodules, returning ``True`` on a cache hit."""
    repository = repository.resolve()
    signature = submodule_signature(repository)
    if signature is None:
        print("[submodules] no pinned submodules in this checkout")
        return False

    namespace = _cache_namespace(repository, cache_namespace)
    paths = cache_paths(cache_root, namespace)
    paths.root.mkdir(parents=True, exist_ok=True)

    with _locked(paths.lock, exclusive=False):
        if _cache_is_current(paths, signature):
            _restore_modules(repository, paths.modules)
            cache_hit = True
        else:
            cache_hit = False

    if cache_hit:
        try:
            _update_submodules(repository, no_fetch=True)
        except subprocess.CalledProcessError:
            print("[submodules] cached metadata was incomplete; refreshing it", file=sys.stderr)
        else:
            print(f"[submodules] cache hit for pinned graph {signature[:12]}")
            return True

    with _locked(paths.lock, exclusive=True):
        if _cache_is_current(paths, signature):
            _restore_modules(repository, paths.modules)
            try:
                _update_submodules(repository, no_fetch=True)
            except subprocess.CalledProcessError:
                print("[submodules] cached metadata was incomplete; refreshing it", file=sys.stderr)
            else:
                print(f"[submodules] cache hit for pinned graph {signature[:12]}")
                return True

        _refresh_cache(repository, paths, signature)
        print(f"[submodules] cache refreshed for pinned graph {signature[:12]}")
        return False


def parser() -> argparse.ArgumentParser:
    result = argparse.ArgumentParser(description=__doc__)
    result.add_argument(
        "--cache-root",
        default=os.environ.get("CPP_CI_SUBMODULE_CACHE_ROOT", "/submodule-cache"),
        help="persistent directory mounted by the CI runner",
    )
    result.add_argument(
        "--cache-namespace",
        default=os.environ.get("GITHUB_REPOSITORY"),
        help="repository identity used to isolate cache entries",
    )
    return result


def main() -> None:
    args = parser().parse_args()
    try:
        prepare(
            Path.cwd(),
            cache_root=Path(args.cache_root),
            cache_namespace=args.cache_namespace,
        )
    except (OSError, SubmoduleCacheError, subprocess.CalledProcessError) as error:
        print(f"[submodules] {error}", file=sys.stderr)
        raise SystemExit(error.returncode if isinstance(error, subprocess.CalledProcessError) else 2)


if __name__ == "__main__":
    main()
