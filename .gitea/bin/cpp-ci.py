#!/usr/bin/env python3
"""Configuration-driven commands used by the shared C++ CI workflow.

Run this script from a consuming repository checkout.  The common repository
may live anywhere; the workflow template uses ``.gitea/ci`` and keeps the
consumer's project policy in ``.ci/cpp-ci.json``.
"""

from __future__ import annotations

import argparse
import os
import subprocess
import sys
from pathlib import Path


COMMON_ROOT = Path(__file__).resolve().parents[1]
if str(COMMON_ROOT) not in sys.path:
    sys.path.insert(0, str(COMMON_ROOT))

from cpp_ci import artifacts, cmake  # noqa: E402
from cpp_ci.config import BuildProfile, ConfigurationError, load_project_configuration  # noqa: E402
from cpp_ci.diff import DiffError, prepare_base_ref  # noqa: E402


def _config_path(args: argparse.Namespace) -> str:
    return args.config or os.environ.get("CPP_CI_CONFIG", ".ci/cpp-ci.json")


def _load(args: argparse.Namespace):
    return load_project_configuration(_config_path(args))


def _profile(config, name: str) -> BuildProfile:
    return config.profile(name)


def _analysis_profile(config) -> BuildProfile:
    build_root = config.resolve_path(str(config.data["cmake"]["build_root"]))
    return BuildProfile("clang-tidy", "clang", "Debug", build_root / "clang-tidy")


def _module_environment(config) -> dict[str, str]:
    environment = os.environ.copy()
    package_path = str(COMMON_ROOT)
    environment["PYTHONPATH"] = package_path + (
        os.pathsep + environment["PYTHONPATH"] if environment.get("PYTHONPATH") else ""
    )
    environment["CPP_CI_CLANG_FORMAT"] = config.tools["clang_format"]
    environment["CPP_CI_CLANG_TIDY"] = config.tools["clang_tidy"]
    environment["CPP_CI_VALGRIND"] = config.tools["valgrind"]
    return environment


def _run_module(config, module: str, arguments: list[str]) -> None:
    command = [sys.executable, "-m", module, *arguments]
    print("+ " + " ".join(command), flush=True)
    subprocess.run(command, cwd=config.project_root, env=_module_environment(config), check=True)


def configure_command(args: argparse.Namespace) -> None:
    config = _load(args)
    cmake.configure(config, _profile(config, args.profile))


def build_command(args: argparse.Namespace) -> None:
    config = _load(args)
    cmake.build(config, _profile(config, args.profile), target=args.target)


def _prepare_diff(config, base: str, files: list[str] | None, verbose: bool) -> None:
    if files is None:
        prepare_base_ref(base, verbose=verbose)


def format_command(args: argparse.Namespace) -> None:
    config = _load(args)
    base = args.base or config.base_ref
    _prepare_diff(config, base, args.files, args.verbose)
    command = ["--base", base, "--report-path", "ci-results/clang-format.json"]
    if args.files:
        command.extend(("--files", *args.files))
    if args.fix:
        command.append("--fix")
    if args.no_gitea:
        command.append("--no-gitea")
    if args.verbose:
        command.append("--verbose")
    for prefix in config.data["analysis"]["excluded_paths"]:
        command.extend(("--exclude", prefix))
    for extension in config.data["analysis"]["extensions"]:
        command.extend(("--extension", extension))
    _run_module(config, "cpp_ci.clang_format", command)


def tidy_command(args: argparse.Namespace) -> None:
    config = _load(args)
    profile = _analysis_profile(config)
    cmake.configure(config, profile)
    cmake.prepare_analysis(config, profile)
    base = args.base or config.base_ref
    _prepare_diff(config, base, args.files, args.verbose)
    cache_root = Path(os.environ.get("CPP_CI_TIDY_CACHE_ROOT", "/clang-tidy-cache"))
    cache_dir = cache_root / config.project_id / profile.name
    command = [
        "--base",
        base,
        "--build-dir",
        str(profile.build_dir),
        "--fail-on",
        config.data["analysis"]["clang_tidy_fail_on"],
        "--jobs",
        str(config.data["parallelism"]["clang_tidy_jobs"]),
        "--cache-dir",
        str(cache_dir),
        "--report-path",
        "ci-results/clang-tidy.json",
    ]
    if args.files:
        command.extend(("--files", *args.files))
    if args.no_gitea:
        command.append("--no-gitea")
    if args.verbose:
        command.append("--verbose")
    for prefix in config.data["analysis"]["excluded_paths"]:
        command.extend(("--exclude", prefix))
    for extension in config.data["analysis"]["extensions"]:
        command.extend(("--extension", extension))
    _run_module(config, "cpp_ci.clang_tidy", command)


def test_command(args: argparse.Namespace) -> None:
    config = _load(args)
    profile = _profile(config, args.profile)
    command = [config.expand(value, profile) for value in config.data["unit_tests"]["command"]]
    executable = config.resolve_path(command[0])
    if not executable.is_file():
        raise ConfigurationError(
            f"unit-test executable for {profile.name} is unavailable: {executable}; restore its runtime artifact first"
        )
    command[0] = str(executable)
    if args.test_args:
        command.extend(args.test_args)
    print("+ " + " ".join(command), flush=True)
    subprocess.run(command, cwd=config.project_root, check=True)


def valgrind_command(args: argparse.Namespace) -> None:
    config = _load(args)
    profile_name = args.profile or config.data["valgrind"]["variant"]
    profile = _profile(config, profile_name)
    test_command = [config.expand(value, profile) for value in config.data["unit_tests"]["command"]]
    executable = config.resolve_path(test_command[0])
    command = ["--executable", str(executable), "--verbose"]
    if args.no_gitea:
        command.append("--no-gitea")
    if config.data["valgrind"].get("generate_suppressions"):
        command.append("--generate-suppressions")
    for value in config.data["valgrind"].get("suppressions", []):
        command.extend(("--suppression", str(config.resolve_path(value))))
    if args.test_args:
        command.extend(("--", *args.test_args))
    _run_module(config, "cpp_ci.valgrind", command)


def artifact_stage_command(args: argparse.Namespace) -> None:
    config = _load(args)
    profile = _profile(config, args.profile)
    output = config.resolve_path(args.archive) if args.archive else None
    artifacts.stage(config, profile, output=output)


def artifact_restore_command(args: argparse.Namespace) -> None:
    config = _load(args)
    profile = _profile(config, args.profile)
    archive = config.resolve_path(args.archive) if args.archive else None
    artifacts.restore(config, profile, archive=archive)


def _runtime_command(config, profile: BuildProfile, *, if_enabled: bool = False) -> list[str] | None:
    runtime = config.data["runtime"]
    if not runtime["enabled"]:
        if if_enabled:
            print("[runtime] disabled by this project's configuration; skipping")
            return None
        raise ConfigurationError("runtime helpers are disabled for this project configuration")
    return [config.expand(value, profile) for value in runtime["command"]]


def runtime_capture_command(args: argparse.Namespace) -> None:
    config = _load(args)
    runtime = config.data["runtime"]
    profile = _profile(config, args.profile or runtime["variant"])
    application = _runtime_command(config, profile, if_enabled=args.if_enabled)
    if application is None:
        return
    command = [
        "capture-window",
        "--output",
        args.output,
        "--warmup",
        str(runtime["capture_warmup_seconds"]),
    ]
    if runtime.get("window_title"):
        command.extend(("--window-title", runtime["window_title"]))
    if runtime.get("screen_size"):
        command.extend(("--screen-size", runtime["screen_size"]))
    for value in runtime["environment"]:
        command.extend(("--environment", value))
    command.extend(("--", *application))
    _run_module(config, "cpp_ci.runtime", command)


def runtime_valgrind_command(args: argparse.Namespace) -> None:
    config = _load(args)
    runtime = config.data["runtime"]
    profile = _profile(config, args.profile or runtime["variant"])
    application = _runtime_command(config, profile, if_enabled=args.if_enabled)
    if application is None:
        return
    command = [
        "run-under-valgrind",
        "--valgrind",
        config.tools["valgrind"],
        "--timeout",
        str(runtime["valgrind_timeout_seconds"]),
        "--headless",
        "--expect-timeout",
    ]
    if config.data["valgrind"].get("generate_suppressions"):
        command.append("--generate-suppressions")
    if runtime.get("screen_size"):
        command.extend(("--screen-size", runtime["screen_size"]))
    for value in config.data["valgrind"].get("suppressions", []):
        command.extend(("--suppression", str(config.resolve_path(value))))
    for value in runtime["environment"]:
        command.extend(("--environment", value))
    command.extend(("--", *application))
    _run_module(config, "cpp_ci.runtime", command)


def show_command(args: argparse.Namespace) -> None:
    config = _load(args)
    profile = _profile(config, args.profile)
    print(f"project_id={config.project_id}")
    print(f"profile={profile.name}")
    print(f"build_dir={profile.build_dir}")
    print(f"compiler={config.tools['toolchains'][profile.compiler]['c']}")
    print(f"cxx_compiler={config.tools['toolchains'][profile.compiler]['cxx']}")


def _add_config_argument(parser: argparse.ArgumentParser) -> None:
    parser.add_argument("--config", help="consumer JSON configuration (default: .ci/cpp-ci.json)")


def _add_profile_argument(parser: argparse.ArgumentParser, *, required: bool = True) -> None:
    parser.add_argument("--profile", required=required, choices=("clang-debug", "clang-release", "gcc-debug", "gcc-release"))


def _add_diff_arguments(parser: argparse.ArgumentParser) -> None:
    parser.add_argument("--base", help="base branch/ref (defaults to project config)")
    parser.add_argument("--files", nargs="+", help="explicit changed C/C++ files; skips base fetch")
    parser.add_argument("--no-gitea", action="store_true", help="do not publish PR review/check results")
    parser.add_argument("--verbose", action="store_true")


def parser() -> argparse.ArgumentParser:
    result = argparse.ArgumentParser(description=__doc__)
    commands = result.add_subparsers(dest="command", required=True)

    configure = commands.add_parser("configure", help="configure one isolated CMake profile")
    _add_config_argument(configure)
    _add_profile_argument(configure)
    configure.set_defaults(handler=configure_command)

    build = commands.add_parser("build", help="build one configured CMake profile with ccache")
    _add_config_argument(build)
    _add_profile_argument(build)
    build.add_argument("--target", help="optional CMake target")
    build.set_defaults(handler=build_command)

    format_check = commands.add_parser("format", help="check changed C/C++ formatting")
    _add_config_argument(format_check)
    _add_diff_arguments(format_check)
    format_check.add_argument("--fix", action="store_true", help="apply clang-format to changed files")
    format_check.set_defaults(handler=format_command)

    tidy = commands.add_parser("tidy", help="configure isolated clang-tidy profile and analyse changed C++")
    _add_config_argument(tidy)
    _add_diff_arguments(tidy)
    tidy.set_defaults(handler=tidy_command)

    test = commands.add_parser("test", help="run a restored unit-test runtime bundle")
    _add_config_argument(test)
    _add_profile_argument(test)
    test.add_argument("test_args", nargs=argparse.REMAINDER, help="arguments passed to the test command after '--'")
    test.set_defaults(handler=test_command)

    valgrind = commands.add_parser("valgrind", help="run configured unit tests under Valgrind")
    _add_config_argument(valgrind)
    _add_profile_argument(valgrind, required=False)
    valgrind.add_argument("--no-gitea", action="store_true")
    valgrind.add_argument("test_args", nargs=argparse.REMAINDER, help="arguments passed to the test command after '--'")
    valgrind.set_defaults(handler=valgrind_command)

    stage = commands.add_parser("artifact-stage", help="create a variant runtime tarball")
    _add_config_argument(stage)
    _add_profile_argument(stage)
    stage.add_argument("--archive", help="path for the generated archive")
    stage.set_defaults(handler=artifact_stage_command)

    restore = commands.add_parser("artifact-restore", help="restore a variant runtime tarball")
    _add_config_argument(restore)
    _add_profile_argument(restore)
    restore.add_argument("--archive", help="path of the downloaded archive")
    restore.set_defaults(handler=artifact_restore_command)

    capture = commands.add_parser("runtime-capture", help="capture a configured optional application under Xvfb")
    _add_config_argument(capture)
    _add_profile_argument(capture, required=False)
    capture.add_argument("--output", default="ci-runtime/application-ui.png")
    capture.add_argument("--if-enabled", action="store_true", help="succeed with a skip when runtime.enabled is false")
    capture.set_defaults(handler=runtime_capture_command)

    runtime_valgrind = commands.add_parser(
        "runtime-valgrind", help="run a configured optional application under Valgrind/Xvfb"
    )
    _add_config_argument(runtime_valgrind)
    _add_profile_argument(runtime_valgrind, required=False)
    runtime_valgrind.add_argument("--if-enabled", action="store_true", help="succeed with a skip when runtime.enabled is false")
    runtime_valgrind.set_defaults(handler=runtime_valgrind_command)

    show = commands.add_parser("show", help="display resolved configuration for one profile")
    _add_config_argument(show)
    _add_profile_argument(show)
    show.set_defaults(handler=show_command)
    return result


def main() -> None:
    args = parser().parse_args()
    try:
        args.handler(args)
    except (ConfigurationError, DiffError, OSError, subprocess.CalledProcessError) as error:
        print(f"[cpp-ci] {error}", file=sys.stderr)
        raise SystemExit(error.returncode if isinstance(error, subprocess.CalledProcessError) else 2)


if __name__ == "__main__":
    main()
