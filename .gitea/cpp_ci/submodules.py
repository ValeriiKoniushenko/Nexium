"""Materialize pinned Git submodules from an append-only object cache.

Every CI job owns its checkout, submodule metadata, and worktrees. The
persistent cache contains only immutable Git objects and a small metadata
snapshot. Restored job-local repositories borrow immutable objects via
``objects/info/alternates`` rather than copying every pack file into each job.
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


CACHE_FORMAT = 2
LEGACY_CACHE_FORMAT = 1
DEFAULT_SUBMODULE_JOBS = 4


class SubmoduleCacheError(RuntimeError):
    """Raised when the persistent submodule cache cannot be used safely."""


@dataclass(frozen=True)
class CachePaths:
    """Persistent paths for one consumer repository's submodule cache."""

    root: Path
    metadata_modules: Path
    objects: Path
    metadata: Path
    lock: Path
    legacy_modules: Path


@dataclass(frozen=True)
class CacheMetadata:
    """Validated cache manifest contents."""

    format: int
    signature: str


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
    gitlink changes, so the top-level gitlinks plus ``.gitmodules`` invalidate
    a recursive metadata cache precisely enough for pinned checkouts.
    """
    entries = _cached_gitlink_entries(repository)
    if not entries:
        return None

    gitmodules = repository / ".gitmodules"
    if not gitmodules.is_file():
        raise SubmoduleCacheError("the checkout has submodule gitlinks but no .gitmodules file")

    # Keep this graph identity stable across cache storage formats, allowing a
    # v1 copied-metadata cache to become a v2 object cache without re-fetching.
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
    """Return traversal-safe, repository-scoped cache paths."""
    root = cache_root.resolve() / hashlib.sha256(namespace.encode()).hexdigest()
    return CachePaths(
        root=root,
        metadata_modules=root / "metadata-modules",
        objects=root / "objects",
        metadata=root / "metadata.json",
        lock=root / "lock",
        # Version 1 stored a complete .git/modules tree here. Keep it as a
        # read-only alternate source when upgrading so existing caches do not
        # need to download all dependencies again.
        legacy_modules=root / "modules",
    )


def _read_metadata(path: Path) -> CacheMetadata | None:
    try:
        data = json.loads(path.read_text(encoding="utf-8"))
    except (FileNotFoundError, json.JSONDecodeError, OSError):
        return None
    if not isinstance(data, dict):
        return None
    cache_format = data.get("format")
    signature = data.get("signature")
    if not isinstance(cache_format, int) or not isinstance(signature, str):
        return None
    return CacheMetadata(cache_format, signature)


def _cache_is_current(paths: CachePaths, signature: str) -> bool:
    metadata = _read_metadata(paths.metadata)
    return (
        metadata is not None
        and metadata.format == CACHE_FORMAT
        and metadata.signature == signature
        and paths.metadata_modules.is_dir()
    )


def _legacy_cache_is_current(paths: CachePaths, signature: str) -> bool:
    metadata = _read_metadata(paths.metadata)
    return (
        metadata is not None
        and metadata.format == LEGACY_CACHE_FORMAT
        and metadata.signature == signature
        and paths.legacy_modules.is_dir()
    )


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


def _remove_directory(path: Path, *, description: str) -> None:
    if not path.exists() and not path.is_symlink():
        return
    if path.is_symlink() or not path.is_dir():
        raise SubmoduleCacheError(f"refusing to remove unexpected {description}: {path}")
    shutil.rmtree(path)


def _copy_metadata(source: Path, destination: Path) -> None:
    """Copy private Git metadata but deliberately leave all objects behind."""
    if source.is_symlink() or not source.is_dir():
        raise SubmoduleCacheError(f"submodule metadata directory does not exist: {source}")
    shutil.copytree(
        source,
        destination,
        symlinks=True,
        ignore=shutil.ignore_patterns("objects"),
    )


def _module_git_directories(modules: Path) -> list[Path]:
    """Find embedded Git directories belonging to initialized modules."""
    if not modules.is_dir():
        return []
    return sorted(
        config.parent
        for config in modules.rglob("config")
        if (config.parent / "HEAD").is_file()
    )


def _object_store_sources(paths: CachePaths, relative_git_dir: Path) -> list[Path]:
    """Return stable object stores for one embedded Git directory."""
    candidates = (
        paths.objects / relative_git_dir / "objects",
        paths.legacy_modules / relative_git_dir / "objects",
    )
    return [candidate.resolve() for candidate in candidates if candidate.is_dir()]


def _write_alternates(object_directory: Path, sources: list[Path]) -> None:
    info = object_directory / "info"
    info.mkdir(parents=True, exist_ok=True)
    alternate_file = info / "alternates"
    if not sources:
        if alternate_file.exists() or alternate_file.is_symlink():
            if alternate_file.is_symlink() or alternate_file.is_file():
                alternate_file.unlink()
            else:
                raise SubmoduleCacheError(f"unexpected alternates path: {alternate_file}")
        return
    alternate_file.write_text("".join(f"{source}\n" for source in sources), encoding="utf-8")


def _metadata_source(paths: CachePaths) -> Path | None:
    if paths.metadata_modules.is_dir():
        return paths.metadata_modules
    if paths.legacy_modules.is_dir():
        return paths.legacy_modules
    return None


def _restore_metadata(repository: Path, paths: CachePaths) -> bool:
    """Restore small private metadata and point it at immutable cache objects."""
    modules = _git_modules_directory(repository)
    _remove_directory(modules, description="Git modules path")

    source = _metadata_source(paths)
    if source is None:
        return False
    _copy_metadata(source, modules)

    for git_dir in _module_git_directories(modules):
        relative = git_dir.relative_to(modules)
        object_directory = git_dir / "objects"
        object_directory.mkdir(parents=True, exist_ok=True)
        _write_alternates(object_directory, _object_store_sources(paths, relative))
    return True


def _remove_job_alternates(object_directory: Path) -> None:
    alternate_file = object_directory / "info" / "alternates"
    if not alternate_file.exists() and not alternate_file.is_symlink():
        return
    if alternate_file.is_symlink() or alternate_file.is_file():
        alternate_file.unlink()
        return
    raise SubmoduleCacheError(f"unexpected alternates path: {alternate_file}")


def _copy_new_objects(source: Path, destination: Path) -> None:
    """Append job-created immutable objects without overwriting cached ones."""
    if not source.is_dir():
        raise SubmoduleCacheError(f"Git object directory does not exist: {source}")
    destination.mkdir(parents=True, exist_ok=True)
    try:
        subprocess.run(
            [
                "cp",
                "-a",
                "--reflink=auto",
                "--no-preserve=ownership",
                "--no-clobber",
                f"{source}/.",
                str(destination),
            ],
            check=True,
        )
    except FileNotFoundError as error:
        raise SubmoduleCacheError("the CI image needs the GNU cp command for the submodule cache") from error


def _merge_job_objects(repository: Path, paths: CachePaths) -> None:
    """Publish only objects written by this job into the append-only cache."""
    modules = _git_modules_directory(repository)
    for git_dir in _module_git_directories(modules):
        object_directory = git_dir / "objects"
        _remove_job_alternates(object_directory)
        destination = paths.objects / git_dir.relative_to(modules) / "objects"
        _copy_new_objects(object_directory, destination)


def _update_submodules(repository: Path, *, no_fetch: bool, jobs: int) -> None:
    _run_git(repository, ["submodule", "sync", "--recursive"])
    command = [
        "submodule",
        "update",
        "--init",
        "--recursive",
        "--depth",
        "1",
        "--jobs",
        str(jobs),
    ]
    if no_fetch:
        command.append("--no-fetch")
    _run_git(repository, command)


def _store_cache(repository: Path, paths: CachePaths, signature: str) -> None:
    modules = _git_modules_directory(repository)
    if not modules.is_dir():
        raise SubmoduleCacheError("submodule update completed without creating Git module metadata")

    # This happens under the exclusive cache lock. The object trees are
    # append-only, while replacement metadata is small and job-private.
    _merge_job_objects(repository, paths)
    paths.root.mkdir(parents=True, exist_ok=True)
    staging = Path(tempfile.mkdtemp(prefix=".staging-", dir=paths.root))
    try:
        staged_modules = staging / "metadata-modules"
        staged_metadata = staging / "metadata.json"
        _copy_metadata(modules, staged_modules)
        staged_metadata.write_text(
            json.dumps({"format": CACHE_FORMAT, "signature": signature}, sort_keys=True) + "\n",
            encoding="utf-8",
        )

        _remove_directory(paths.metadata_modules, description="cached submodule metadata")
        staged_modules.replace(paths.metadata_modules)
        if paths.metadata.exists() or paths.metadata.is_symlink():
            if paths.metadata.is_symlink() or not paths.metadata.is_file():
                raise SubmoduleCacheError(f"refusing to replace unexpected cache metadata path: {paths.metadata}")
            paths.metadata.unlink()
        staged_metadata.replace(paths.metadata)
    finally:
        shutil.rmtree(staging, ignore_errors=True)


def _try_cached_update(repository: Path, paths: CachePaths, *, jobs: int) -> bool:
    if not _restore_metadata(repository, paths):
        return False
    _update_submodules(repository, no_fetch=True, jobs=jobs)
    return True


def _refresh_cache(repository: Path, paths: CachePaths, signature: str, *, jobs: int) -> None:
    _restore_metadata(repository, paths)
    _update_submodules(repository, no_fetch=False, jobs=jobs)
    _store_cache(repository, paths, signature)


def prepare(
        repository: Path,
        *,
        cache_root: Path,
        cache_namespace: str | None = None,
        jobs: int = DEFAULT_SUBMODULE_JOBS,
) -> bool:
    """Materialize pinned submodules, returning ``True`` on a cache hit."""
    if jobs < 1:
        raise SubmoduleCacheError("submodule cache jobs must be at least one")

    repository = repository.resolve()
    signature = submodule_signature(repository)
    if signature is None:
        print("[submodules] no pinned submodules in this checkout")
        return False

    namespace = _cache_namespace(repository, cache_namespace)
    paths = cache_paths(cache_root, namespace)
    paths.root.mkdir(parents=True, exist_ok=True)

    # Keep the shared lock until checkout has read every alternate object. A
    # concurrent refresh may append objects and replace metadata afterwards,
    # but it never removes the object stores a completed job depends on.
    with _locked(paths.lock, exclusive=False):
        if _cache_is_current(paths, signature):
            try:
                _try_cached_update(repository, paths, jobs=jobs)
            except subprocess.CalledProcessError:
                print("[submodules] cached objects were incomplete; refreshing them", file=sys.stderr)
            else:
                print(f"[submodules] cache hit for pinned graph {signature[:12]}")
                return True

    with _locked(paths.lock, exclusive=True):
        # Another job may have refreshed the cache while this job waited.
        if _cache_is_current(paths, signature):
            try:
                _try_cached_update(repository, paths, jobs=jobs)
            except subprocess.CalledProcessError:
                print("[submodules] cached objects were incomplete; refreshing them", file=sys.stderr)
            else:
                print(f"[submodules] cache hit for pinned graph {signature[:12]}")
                return True

        # Upgrade the previous copied-metadata cache without downloading its
        # object packs again. Its immutable objects remain a read-only
        # alternate source after the manifest is converted to format 2.
        if _legacy_cache_is_current(paths, signature):
            try:
                _try_cached_update(repository, paths, jobs=jobs)
            except subprocess.CalledProcessError:
                print("[submodules] legacy cached objects were incomplete; refreshing them", file=sys.stderr)
            else:
                _store_cache(repository, paths, signature)
                print(f"[submodules] cache hit for pinned graph {signature[:12]} (migrated)")
                return True

        _refresh_cache(repository, paths, signature, jobs=jobs)
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
    result.add_argument(
        "--jobs",
        type=int,
        default=os.environ.get("CPP_CI_SUBMODULE_JOBS", DEFAULT_SUBMODULE_JOBS),
        help="parallel submodule update work, including cached worktree checkout",
    )
    return result


def main() -> None:
    args = parser().parse_args()
    try:
        prepare(
            Path.cwd(),
            cache_root=Path(args.cache_root),
            cache_namespace=args.cache_namespace,
            jobs=args.jobs,
        )
    except (OSError, SubmoduleCacheError, subprocess.CalledProcessError) as error:
        print(f"[submodules] {error}", file=sys.stderr)
        raise SystemExit(error.returncode if isinstance(error, subprocess.CalledProcessError) else 2)


if __name__ == "__main__":
    main()
