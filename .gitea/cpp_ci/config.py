"""Configuration loading and profile resolution for the shared C++ CI tools.

The common repository owns tool defaults; each consuming repository owns a small
JSON file containing only its build/test policy.  JSON is used deliberately so
the runner needs no third-party Python package.
"""

from __future__ import annotations

import copy
import json
import re
from dataclasses import dataclass
from pathlib import Path
from typing import Any


class ConfigurationError(RuntimeError):
    """Raised when a project CI configuration cannot be used safely."""


PROJECT_ID_RE = re.compile(r"^[A-Za-z0-9][A-Za-z0-9_.-]*$")
PROFILE_RE = re.compile(r"^(clang|gcc)-(debug|release)$")


def common_root() -> Path:
    """Return the root of the checked-out shared-CI repository."""
    return Path(__file__).resolve().parent.parent


def _read_json(path: Path) -> dict[str, Any]:
    try:
        value = json.loads(path.read_text(encoding="utf-8"))
    except FileNotFoundError as error:
        raise ConfigurationError(f"configuration file does not exist: {path}") from error
    except json.JSONDecodeError as error:
        raise ConfigurationError(f"invalid JSON in {path}: {error}") from error
    if not isinstance(value, dict):
        raise ConfigurationError(f"configuration root must be an object: {path}")
    return value


def _merge(defaults: Any, override: Any) -> Any:
    """Recursively merge JSON objects without mutating either input."""
    if isinstance(defaults, dict) and isinstance(override, dict):
        merged = {key: copy.deepcopy(value) for key, value in defaults.items()}
        for key, value in override.items():
            merged[key] = _merge(merged[key], value) if key in merged else copy.deepcopy(value)
        return merged
    return copy.deepcopy(override)


@dataclass(frozen=True)
class BuildProfile:
    """One independent compiler/build-type directory."""

    name: str
    compiler: str
    build_type: str
    build_dir: Path


@dataclass(frozen=True)
class ProjectConfiguration:
    """Merged defaults and project policy, rooted at the consumer checkout."""

    data: dict[str, Any]
    project_root: Path
    source_path: Path

    @property
    def project_id(self) -> str:
        return str(self.data["project_id"])

    @property
    def base_ref(self) -> str:
        return str(self.data["base_ref"])

    @property
    def tools(self) -> dict[str, Any]:
        return self.data["tools"]

    def profile(self, name: str) -> BuildProfile:
        match = PROFILE_RE.fullmatch(name)
        if not match:
            valid = ", ".join(("clang-debug", "clang-release", "gcc-debug", "gcc-release"))
            raise ConfigurationError(f"unknown profile {name!r}; expected one of: {valid}")
        compiler, flavour = match.groups()
        build_root = self.resolve_path(str(self.data["cmake"]["build_root"]))
        return BuildProfile(
            name=name,
            compiler=compiler,
            build_type=flavour.capitalize(),
            build_dir=build_root / name,
        )

    def resolve_path(self, value: str) -> Path:
        path = Path(value)
        return path if path.is_absolute() else self.project_root / path

    def expand(self, value: str, profile: BuildProfile) -> str:
        relative_build_dir = profile.build_dir.relative_to(self.project_root).as_posix()
        return value.format(
            variant=profile.name,
            compiler=profile.compiler,
            build_type=profile.build_type,
            build_dir=relative_build_dir,
        )


def _validate(config: dict[str, Any], source_path: Path) -> None:
    if config.get("schema_version") != 1:
        raise ConfigurationError(
            f"{source_path}: schema_version must be 1; got {config.get('schema_version')!r}"
        )
    project_id = config.get("project_id")
    if not isinstance(project_id, str) or not PROJECT_ID_RE.fullmatch(project_id):
        raise ConfigurationError(
            f"{source_path}: project_id must contain only letters, digits, '.', '_' or '-'"
        )
    if not isinstance(config.get("base_ref"), str) or not config["base_ref"]:
        raise ConfigurationError(f"{source_path}: base_ref must be a non-empty string")

    for path in (("tools",), ("cmake",), ("unit_tests",), ("analysis",), ("valgrind",), ("artifacts",)):
        current: Any = config
        for key in path:
            if not isinstance(current, dict) or key not in current:
                raise ConfigurationError(f"{source_path}: missing required section {'.'.join(path)}")
            current = current[key]
        if not isinstance(current, dict):
            raise ConfigurationError(f"{source_path}: section {'.'.join(path)} must be an object")

    toolchains = config["tools"].get("toolchains")
    if not isinstance(toolchains, dict):
        raise ConfigurationError(f"{source_path}: tools.toolchains must be an object")
    for compiler in ("clang", "gcc"):
        toolchain = toolchains.get(compiler)
        if not isinstance(toolchain, dict) or not all(
            isinstance(toolchain.get(key), str) and toolchain[key] for key in ("c", "cxx")
        ):
            raise ConfigurationError(
                f"{source_path}: tools.toolchains.{compiler} must define non-empty c and cxx commands"
            )
    for key in ("ccache", "clang_format", "clang_tidy", "valgrind"):
        if not isinstance(config["tools"].get(key), str) or not config["tools"][key]:
            raise ConfigurationError(f"{source_path}: tools.{key} must be a non-empty command string")

    cmake = config["cmake"]
    for key in ("command", "generator", "source_dir", "build_root"):
        if not isinstance(cmake.get(key), str) or not cmake[key]:
            raise ConfigurationError(f"{source_path}: cmake.{key} must be a non-empty string")
    for key in ("arguments", "prepare_targets"):
        if not isinstance(cmake.get(key), list) or not all(isinstance(item, str) for item in cmake[key]):
            raise ConfigurationError(f"{source_path}: cmake.{key} must be an array of strings")
    profile_arguments = cmake.get("profile_arguments")
    if not isinstance(profile_arguments, dict) or not all(
        PROFILE_RE.fullmatch(key) and isinstance(value, list) and all(isinstance(item, str) for item in value)
        for key, value in profile_arguments.items()
    ):
        raise ConfigurationError(
            f"{source_path}: cmake.profile_arguments must map supported profiles to arrays of strings"
        )

    command = config["unit_tests"].get("command")
    if not isinstance(command, list) or not command or not all(isinstance(item, str) for item in command):
        raise ConfigurationError(f"{source_path}: unit_tests.command must be a non-empty array of strings")
    artifact_paths = config["artifacts"].get("paths")
    if not isinstance(artifact_paths, list) or not all(isinstance(item, str) for item in artifact_paths):
        raise ConfigurationError(f"{source_path}: artifacts.paths must be an array of strings")

    analysis = config["analysis"]
    if not isinstance(analysis.get("excluded_paths"), list) or not all(
        isinstance(item, str) for item in analysis["excluded_paths"]
    ):
        raise ConfigurationError(f"{source_path}: analysis.excluded_paths must be an array of strings")
    if not isinstance(analysis.get("extensions"), list) or not analysis["extensions"] or not all(
        isinstance(item, str) for item in analysis["extensions"]
    ):
        raise ConfigurationError(f"{source_path}: analysis.extensions must be a non-empty array of strings")
    if analysis.get("clang_tidy_fail_on") not in {"error", "warning", "note"}:
        raise ConfigurationError(f"{source_path}: analysis.clang_tidy_fail_on must be error, warning, or note")

    parallelism = config.get("parallelism", {})
    if not isinstance(parallelism, dict) or not all(
        isinstance(parallelism.get(key), int) and parallelism[key] > 0
        for key in ("build_job_divisor", "clang_tidy_jobs")
    ):
        raise ConfigurationError(
            f"{source_path}: parallelism.build_job_divisor and parallelism.clang_tidy_jobs must be positive integers"
        )

    valgrind = config["valgrind"]
    if valgrind.get("variant") not in {"clang-debug", "clang-release", "gcc-debug", "gcc-release"}:
        raise ConfigurationError(f"{source_path}: valgrind.variant must name a supported build profile")
    if not isinstance(valgrind.get("suppressions"), list) or not all(
        isinstance(item, str) for item in valgrind["suppressions"]
    ):
        raise ConfigurationError(f"{source_path}: valgrind.suppressions must be an array of strings")
    if not isinstance(valgrind.get("generate_suppressions"), bool):
        raise ConfigurationError(f"{source_path}: valgrind.generate_suppressions must be a boolean")

    runtime = config.get("runtime", {})
    if not isinstance(runtime, dict) or not isinstance(runtime.get("enabled"), bool):
        raise ConfigurationError(f"{source_path}: runtime.enabled must be a boolean")
    if runtime["enabled"]:
        command = runtime.get("command")
        if not isinstance(command, list) or not command or not all(isinstance(item, str) for item in command):
            raise ConfigurationError(f"{source_path}: enabled runtime.command must be a non-empty array of strings")
        if runtime.get("variant") not in {"clang-debug", "clang-release", "gcc-debug", "gcc-release"}:
            raise ConfigurationError(f"{source_path}: runtime.variant must name a supported build profile")
        if not isinstance(runtime.get("environment"), list) or not all(
            isinstance(item, str) and "=" in item for item in runtime["environment"]
        ):
            raise ConfigurationError(f"{source_path}: runtime.environment must be an array of NAME=VALUE strings")


def load_project_configuration(config_path: str | Path, *, project_root: Path | None = None) -> ProjectConfiguration:
    """Load central defaults plus one consumer configuration file."""
    source_path = Path(config_path).resolve()
    defaults_path = common_root() / "config" / "toolchains.json"
    merged = _merge(_read_json(defaults_path), _read_json(source_path))
    _validate(merged, source_path)
    return ProjectConfiguration(
        data=merged,
        project_root=(project_root or Path.cwd()).resolve(),
        source_path=source_path,
    )
