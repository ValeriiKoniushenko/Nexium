"""CMake/ccache operations shared by all C++ CI consumers."""

from __future__ import annotations

import os
import shlex
import shutil
import subprocess
from pathlib import Path

from .config import BuildProfile, ConfigurationError, ProjectConfiguration


def _command_text(command: list[str]) -> str:
    return shlex.join(command)


def _run(command: list[str], *, cwd: Path, environment: dict[str, str] | None = None) -> None:
    print(f"+ {_command_text(command)}", flush=True)
    subprocess.run(command, cwd=cwd, env=environment, check=True)


def _require_executable(command: str, label: str) -> None:
    if shutil.which(command) is None:
        raise ConfigurationError(
            f"{label} command {command!r} is unavailable. The common CI image must provide it."
        )


def _build_parallelism(config: ProjectConfiguration) -> int:
    requested = config.data["parallelism"].get("build_jobs", 0)
    if isinstance(requested, int) and requested > 0:
        return requested
    divisor = config.data["parallelism"].get("build_job_divisor", 4)
    if not isinstance(divisor, int) or divisor < 1:
        raise ConfigurationError("parallelism.build_job_divisor must be a positive integer")
    return max(1, (os.cpu_count() or 1) // divisor)


def _ccache_environment(config: ProjectConfiguration) -> dict[str, str]:
    environment = os.environ.copy()
    ccache_dir = config.data["tools"].get("ccache_dir")
    if isinstance(ccache_dir, str) and ccache_dir:
        environment["CCACHE_DIR"] = ccache_dir
    return environment


def show_toolchain(config: ProjectConfiguration, profile: BuildProfile) -> None:
    """Validate tool availability and print versions selected by common policy."""
    tools = config.tools
    toolchain = tools["toolchains"][profile.compiler]
    commands = (
        ("C compiler", toolchain["c"]),
        ("C++ compiler", toolchain["cxx"]),
        ("CMake", config.data["cmake"]["command"]),
        ("ccache", tools["ccache"]),
    )
    for label, command in commands:
        _require_executable(command, label)
        completed = subprocess.run([command, "--version"], capture_output=True, text=True)
        output = (completed.stdout or completed.stderr).strip().splitlines()
        version = output[0] if output else f"exit code {completed.returncode}"
        print(f"[toolchain] {label}: {command} — {version}")
        if completed.returncode != 0:
            raise ConfigurationError(f"{label} command {command!r} failed its version check")


def configure(config: ProjectConfiguration, profile: BuildProfile) -> None:
    """Configure one isolated profile with mandatory ccache launchers."""
    show_toolchain(config, profile)
    cmake = config.data["cmake"]
    tools = config.tools
    toolchain = tools["toolchains"][profile.compiler]
    source_dir = config.resolve_path(cmake["source_dir"])
    if not source_dir.is_dir():
        raise ConfigurationError(f"CMake source directory does not exist: {source_dir}")

    profile.build_dir.mkdir(parents=True, exist_ok=True)
    command = [
        cmake["command"],
        "-S",
        str(source_dir),
        "-B",
        str(profile.build_dir),
        "-G",
        cmake["generator"],
        f"-DCMAKE_BUILD_TYPE={profile.build_type}",
        "-DCMAKE_EXPORT_COMPILE_COMMANDS=ON",
        f"-DCMAKE_C_COMPILER={toolchain['c']}",
        f"-DCMAKE_CXX_COMPILER={toolchain['cxx']}",
        "-DCMAKE_C_COMPILER_LAUNCHER=ccache",
        "-DCMAKE_CXX_COMPILER_LAUNCHER=ccache",
        *cmake["arguments"],
        *cmake["profile_arguments"].get(profile.name, []),
    ]
    _run(command, cwd=config.project_root, environment=_ccache_environment(config))


def build(config: ProjectConfiguration, profile: BuildProfile, *, target: str | None = None) -> None:
    """Build one profile and expose ccache statistics before and after it."""
    cmake = config.data["cmake"]
    ccache = config.tools["ccache"]
    environment = _ccache_environment(config)
    _require_executable(ccache, "ccache")
    print("[ccache] statistics before build")
    _run([ccache, "--show-stats"], cwd=config.project_root, environment=environment)
    command = [cmake["command"], "--build", str(profile.build_dir), "--parallel", str(_build_parallelism(config))]
    if target:
        command.extend(("--target", target))
    _run(command, cwd=config.project_root, environment=environment)
    print("[ccache] statistics after build")
    _run([ccache, "--show-stats"], cwd=config.project_root, environment=environment)


def prepare_analysis(config: ProjectConfiguration, profile: BuildProfile) -> None:
    """Build project-declared generated-source prerequisites for static analysis."""
    for target in config.data["cmake"]["prepare_targets"]:
        build(config, profile, target=target)
