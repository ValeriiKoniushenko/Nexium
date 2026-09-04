"""Stage and restore small, variant-specific runtime artifacts safely."""

from __future__ import annotations

import glob
import io
import json
import tarfile
from pathlib import Path

from .config import BuildProfile, ConfigurationError, ProjectConfiguration


def archive_path(project_root: Path, profile: BuildProfile) -> Path:
    return project_root / "ci-artifacts" / f"cpp-ci-runtime-{profile.name}.tar.gz"


def compilation_database_archive_path(project_root: Path, profile: BuildProfile) -> Path:
    return project_root / "ci-artifacts" / f"cpp-ci-compile-commands-{profile.name}.tar.gz"


def _compilation_database_manifest_path(project_root: Path, profile: BuildProfile) -> Path:
    return project_root / "ci-artifacts" / f"cpp-ci-compile-commands-{profile.name}.json"


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


def _restore_archive(project_root: Path, source: Path) -> None:
    if not source.is_file():
        raise ConfigurationError(f"artifact does not exist: {source}")
    with tarfile.open(source, "r:gz") as bundle:
        for member in bundle.getmembers():
            destination = _safe_member_path(project_root, member.name)
            if member.issym() or member.islnk() or not member.isfile():
                raise ConfigurationError(f"artifact contains unsupported member: {member.name!r}")
            destination.parent.mkdir(parents=True, exist_ok=True)
            extracted = bundle.extractfile(member)
            if extracted is None:
                raise ConfigurationError(f"cannot read artifact member: {member.name!r}")
            with destination.open("wb") as output:
                output.write(extracted.read())
            destination.chmod(member.mode)
            print(f"[artifact] restore {member.name}")


def restore(config: ProjectConfiguration, profile: BuildProfile, *, archive: Path | None = None) -> None:
    """Restore a runtime bundle into this job's fresh checkout."""
    _restore_archive(config.project_root, archive or archive_path(config.project_root, profile))


def stage_compilation_database(
        config: ProjectConfiguration,
        profile: BuildProfile,
        *,
        output: Path | None = None,
) -> Path:
    """Archive one build's compilation database for a later clang-tidy job."""
    database = profile.build_dir / "compile_commands.json"
    if not database.is_file():
        raise ConfigurationError(f"compilation database does not exist for {profile.name}: {database}")
    try:
        database_name = database.relative_to(config.project_root).as_posix()
    except ValueError as error:
        raise ConfigurationError(f"compilation database lies outside the checkout: {database}") from error

    destination = output or compilation_database_archive_path(config.project_root, profile)
    destination.parent.mkdir(parents=True, exist_ok=True)
    manifest_name = _compilation_database_manifest_path(config.project_root, profile).relative_to(
        config.project_root
    ).as_posix()
    manifest = json.dumps(
        {"project_root": str(config.project_root), "database": database_name}, sort_keys=True
    ).encode()
    with tarfile.open(destination, "w:gz", format=tarfile.PAX_FORMAT) as archive:
        archive.add(database, arcname=database_name, recursive=False)
        # Code generators commonly place headers beside their source inputs.
        # A later clang-tidy job needs those headers, but not the full CMake
        # build tree that produced them.
        for generated in _generated_source_files(config.project_root):
            generated_name = generated.relative_to(config.project_root).as_posix()
            archive.add(generated, arcname=generated_name, recursive=False)
            print(f"[artifact] add {generated_name}")
        manifest_info = tarfile.TarInfo(manifest_name)
        manifest_info.size = len(manifest)
        manifest_info.mode = 0o600
        archive.addfile(manifest_info, io.BytesIO(manifest))
        print(f"[artifact] add {database_name}")
    print(f"[artifact] created {destination.relative_to(config.project_root)}")
    return destination


def _generated_source_files(project_root: Path) -> list[Path]:
    excluded_roots = {".git", "build", "ci-artifacts", "dependencies"}
    return sorted(
        candidate
        for candidate in project_root.rglob("*.generated.*")
        if candidate.is_file()
        and candidate.relative_to(project_root).parts
        and candidate.relative_to(project_root).parts[0] not in excluded_roots
    )


def _relocate_checkout_paths(value: object, source_root: str, destination_root: str) -> object:
    if isinstance(value, str):
        return value.replace(source_root, destination_root)
    if isinstance(value, list):
        return [_relocate_checkout_paths(item, source_root, destination_root) for item in value]
    if isinstance(value, dict):
        return {key: _relocate_checkout_paths(item, source_root, destination_root) for key, item in value.items()}
    return value


def restore_compilation_database(
        config: ProjectConfiguration,
        profile: BuildProfile,
        *,
        archive: Path | None = None,
) -> Path:
    """Restore and relocate an archived compilation database into this checkout."""
    _restore_archive(
        config.project_root,
        archive or compilation_database_archive_path(config.project_root, profile),
    )
    manifest_path = _compilation_database_manifest_path(config.project_root, profile)
    try:
        manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
    except (OSError, json.JSONDecodeError) as error:
        raise ConfigurationError(f"invalid compilation-database artifact manifest: {manifest_path}") from error
    finally:
        manifest_path.unlink(missing_ok=True)

    source_root = manifest.get("project_root") if isinstance(manifest, dict) else None
    database_name = manifest.get("database") if isinstance(manifest, dict) else None
    if not isinstance(source_root, str) or not isinstance(database_name, str):
        raise ConfigurationError("compilation-database artifact manifest is incomplete")
    database = _safe_member_path(config.project_root, database_name)
    try:
        commands = json.loads(database.read_text(encoding="utf-8"))
    except (OSError, json.JSONDecodeError) as error:
        raise ConfigurationError(f"invalid restored compilation database: {database}") from error
    relocated = _relocate_checkout_paths(commands, source_root, str(config.project_root))
    database.write_text(json.dumps(relocated, indent=2) + "\n", encoding="utf-8")
    print(f"[artifact] relocated {database.relative_to(config.project_root)}")
    return database
