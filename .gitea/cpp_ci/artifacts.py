"""Stage and restore small, variant-specific runtime artifacts safely."""

from __future__ import annotations

import glob
import tarfile
from pathlib import Path

from .config import BuildProfile, ConfigurationError, ProjectConfiguration


def archive_path(project_root: Path, profile: BuildProfile) -> Path:
    return project_root / "ci-artifacts" / f"cpp-ci-runtime-{profile.name}.tar.gz"


def _matching_files(config: ProjectConfiguration, profile: BuildProfile) -> list[Path]:
    files: list[Path] = []
    seen: set[Path] = set()
    for configured_pattern in config.data["artifacts"]["paths"]:
        pattern = config.expand(configured_pattern, profile)
        absolute_pattern = str(config.project_root / pattern)
        matches = [Path(match) for match in glob.glob(absolute_pattern, recursive=True)]
        if not matches:
            raise ConfigurationError(
                f"artifact pattern did not match any path for {profile.name}: {configured_pattern!r}"
            )
        for match in matches:
            candidates = list(match.rglob("*")) if match.is_dir() else [match]
            for candidate in candidates:
                if candidate.is_file() and candidate not in seen:
                    seen.add(candidate)
                    files.append(candidate)
    return files


def stage(config: ProjectConfiguration, profile: BuildProfile, *, output: Path | None = None) -> Path:
    """Create a compressed artifact without sharing build directories across jobs."""
    destination = output or archive_path(config.project_root, profile)
    destination.parent.mkdir(parents=True, exist_ok=True)
    files = _matching_files(config, profile)
    if not files:
        raise ConfigurationError(f"no runtime files selected for {profile.name}")
    with tarfile.open(destination, "w:gz", format=tarfile.PAX_FORMAT) as archive:
        # Runtime bundles may include symlinked shared libraries.  The resolved
        # target was checked to be under the checkout above, so store its bytes
        # as a regular file and keep restore independent of host symlink layout.
        archive.dereference = True
        for path in files:
            try:
                arcname = path.resolve().relative_to(config.project_root).as_posix()
            except ValueError as error:
                raise ConfigurationError(f"artifact lies outside the checkout: {path}") from error
            archive.add(path, arcname=arcname, recursive=False)
            print(f"[artifact] add {arcname}")
    print(f"[artifact] created {destination.relative_to(config.project_root)}")
    return destination


def _safe_member_path(project_root: Path, name: str) -> Path:
    candidate = (project_root / name).resolve()
    try:
        candidate.relative_to(project_root)
    except ValueError as error:
        raise ConfigurationError(f"unsafe path in runtime artifact: {name!r}") from error
    return candidate


def restore(config: ProjectConfiguration, profile: BuildProfile, *, archive: Path | None = None) -> None:
    """Restore a runtime bundle into this job's fresh checkout."""
    source = archive or archive_path(config.project_root, profile)
    if not source.is_file():
        raise ConfigurationError(f"runtime artifact does not exist: {source}")
    with tarfile.open(source, "r:gz") as bundle:
        for member in bundle.getmembers():
            destination = _safe_member_path(config.project_root, member.name)
            if member.issym() or member.islnk() or not member.isfile():
                raise ConfigurationError(f"runtime artifact contains unsupported member: {member.name!r}")
            destination.parent.mkdir(parents=True, exist_ok=True)
            extracted = bundle.extractfile(member)
            if extracted is None:
                raise ConfigurationError(f"cannot read runtime artifact member: {member.name!r}")
            with destination.open("wb") as output:
                output.write(extracted.read())
            destination.chmod(member.mode)
            print(f"[artifact] restore {member.name}")
