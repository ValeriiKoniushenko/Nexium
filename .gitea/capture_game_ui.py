#!/usr/bin/env python3
"""Capture a game's UI and publish it to the pull request that ran CI."""

from __future__ import annotations

import argparse
import json
import os
import re
import shlex
import subprocess
import sys
import time
from dataclasses import dataclass, field
from pathlib import Path

from gitea_client import GiteaClient, review_marker


WARM_UP_SECONDS = 5.0
DISPLAY = ":99"
DEFAULT_SCREENSHOT_NAME = "game-ui.png"
DEFAULT_SCREEN_SIZE = (1920, 1080)
WINDOW_CACHE_PATH = Path("data/cache/RootWindow.json")
CPU_CLOCK_TICKS = int(os.sysconf("SC_CLK_TCK"))
BUILD_TOOL_COMMANDS = (
    ("clang", ("clang", "--version")),
    ("gcc", ("gcc", "--version")),
    ("cmake", ("cmake", "--version")),
    ("ninja", ("ninja", "--version")),
    ("ccache", ("ccache", "--version")),
    ("mold", ("mold", "--version")),
)


def get_screen_size() -> tuple[int, int]:
    """Match Xvfb to the cached GLFW window so no root-window padding is captured."""
    try:
        cache = json.loads(WINDOW_CACHE_PATH.read_text())
        size = cache["_size"]
        width = int(size["width"])
        height = int(size["height"])
        if width > 0 and height > 0:
            return width, height
    except (OSError, ValueError, KeyError, TypeError):
        pass

    return DEFAULT_SCREEN_SIZE


def collect_build_tool_versions() -> list[tuple[str, str]]:
    """Return first-line version output from the tools installed in this CI container."""
    versions = []
    for name, command in BUILD_TOOL_COMMANDS:
        try:
            result = subprocess.run(command, capture_output=True, text=True, timeout=10)
        except (OSError, subprocess.TimeoutExpired) as error:
            versions.append((name, f"unavailable: {error}"))
            continue

        output = (result.stdout or result.stderr).strip()
        if result.returncode != 0:
            versions.append((name, f"unavailable: exit code {result.returncode}"))
        elif output:
            versions.append((name, output.splitlines()[0]))
        else:
            versions.append((name, "version command produced no output"))
    return versions


@dataclass
class CmakeBuildConfiguration:
    cache_path: Path
    configure_command: str
    details: list[tuple[str, str]]


def find_cmake_cache(executable: Path) -> Path | None:
    """Find the build directory owning an executable by walking to CMakeCache.txt."""
    for directory in executable.resolve().parents:
        cache_path = directory / "CMakeCache.txt"
        if cache_path.is_file():
            return cache_path
    return None


def read_cmake_cache(cache_path: Path) -> dict[str, str]:
    values = {}
    for line in cache_path.read_text(errors="replace").splitlines():
        key_with_type, separator, value = line.partition("=")
        if not separator or key_with_type.startswith(("//", "#")):
            continue
        key, _, _ = key_with_type.partition(":")
        values[key] = value
    return values


def display_path(path: Path) -> str:
    try:
        return str(path.relative_to(Path.cwd())) or "."
    except ValueError:
        return str(path)


def detect_compiler_launcher(build_directory: Path) -> str | None:
    build_file = build_directory / "build.ninja"
    try:
        match = re.search(r"^  LAUNCHER = (.+)$", build_file.read_text(), flags=re.MULTILINE)
    except OSError:
        return None
    return match.group(1).strip() if match else None


def collect_cmake_build_configuration(executable: Path) -> CmakeBuildConfiguration | None:
    """Reconstruct the screenshot build's CMake configuration from its cache."""
    cache_path = find_cmake_cache(executable)
    if cache_path is None:
        return None

    values = read_cmake_cache(cache_path)
    build_path = cache_path.parent
    build_directory = display_path(build_path)
    command = ["cmake", "-S", ".", "-B", build_directory]
    generator = values.get("CMAKE_GENERATOR")
    if generator:
        command.extend(("-G", generator))

    compile_commands = values.get("CMAKE_EXPORT_COMPILE_COMMANDS")
    if not compile_commands and (build_path / "compile_commands.json").is_file():
        compile_commands = "ON"
    compiler_launcher = (
        values.get("CMAKE_C_COMPILER_LAUNCHER") or detect_compiler_launcher(build_path)
    )
    cxx_compiler_launcher = values.get("CMAKE_CXX_COMPILER_LAUNCHER") or compiler_launcher
    cmake_options = (
        ("CMAKE_BUILD_TYPE", values.get("CMAKE_BUILD_TYPE")),
        ("CMAKE_EXPORT_COMPILE_COMMANDS", compile_commands),
        ("CMAKE_C_COMPILER", values.get("CMAKE_C_COMPILER")),
        ("CMAKE_CXX_COMPILER", values.get("CMAKE_CXX_COMPILER")),
        ("CMAKE_C_COMPILER_LAUNCHER", compiler_launcher),
        ("CMAKE_CXX_COMPILER_LAUNCHER", cxx_compiler_launcher),
    )
    for name, value in cmake_options:
        if value:
            command.append(f"-D{name}={value}")

    details = [("Build directory", build_directory)]
    details.extend(
        (label, value)
        for label, value in (
            ("Generator", generator),
            ("Build type", values.get("CMAKE_BUILD_TYPE") or "default"),
            ("Compile commands", compile_commands),
            ("C compiler", values.get("CMAKE_C_COMPILER")),
            ("C++ compiler", values.get("CMAKE_CXX_COMPILER")),
            ("C compiler launcher", compiler_launcher),
            ("C++ compiler launcher", cxx_compiler_launcher),
        )
        if value
    )
    return CmakeBuildConfiguration(cache_path, shlex.join(command), details)


def markdown_inline(value: str) -> str:
    """Keep command output and errors on one safe Markdown line."""
    return value.replace("`", "'").replace("\r", " ").replace("\n", " ")


def executable_key(executable_name: str) -> str:
    """Build a stable identifier for this executable's PR artifacts."""
    dashed_name = re.sub(r"(?<=[a-z0-9])(?=[A-Z])", "-", executable_name)
    key = "".join(char.lower() if char.isalnum() else "-" for char in dashed_name)
    return key.strip("-") or "game"


def format_bytes(value: int) -> str:
    if value <= 0:
        return "unavailable"

    units = ("B", "KiB", "MiB", "GiB")
    size = float(value)
    for unit in units:
        if size < 1024 or unit == units[-1]:
            return f"{size:.1f} {unit}"
        size /= 1024
    return "unavailable"


@dataclass
class RunStatistics:
    started_at: float | None = None
    elapsed_seconds: float | None = None
    cpu_seconds: float | None = None
    peak_rss_bytes: int = 0
    initial_cpu_seconds: float | None = field(default=None, repr=False)
    last_cpu_seconds: float | None = field(default=None, repr=False)


def read_process_usage(pid: int) -> tuple[float, int] | None:
    """Read CPU time and resident memory for one Linux process from /proc."""
    try:
        stat_fields = Path(f"/proc/{pid}/stat").read_text().rsplit(")", maxsplit=1)[1].split()
        cpu_seconds = (int(stat_fields[11]) + int(stat_fields[12])) / CPU_CLOCK_TICKS
        status = Path(f"/proc/{pid}/status").read_text().splitlines()
    except (OSError, IndexError, ValueError):
        return None

    memory_bytes = 0
    for line in status:
        if line.startswith(("VmHWM:", "VmRSS:")):
            parts = line.split()
            if len(parts) >= 2:
                memory_bytes = max(memory_bytes, int(parts[1]) * 1024)
    return cpu_seconds, memory_bytes


def record_process_usage(process: subprocess.Popen[object], statistics: RunStatistics) -> None:
    usage = read_process_usage(process.pid)
    if usage is None:
        return

    cpu_seconds, resident_memory = usage
    if statistics.initial_cpu_seconds is None:
        statistics.initial_cpu_seconds = cpu_seconds
    statistics.last_cpu_seconds = cpu_seconds
    statistics.peak_rss_bytes = max(statistics.peak_rss_bytes, resident_memory)


def finish_statistics(statistics: RunStatistics) -> None:
    if statistics.started_at is not None:
        statistics.elapsed_seconds = max(0.0, time.monotonic() - statistics.started_at)
    if statistics.initial_cpu_seconds is not None and statistics.last_cpu_seconds is not None:
        statistics.cpu_seconds = max(0.0, statistics.last_cpu_seconds - statistics.initial_cpu_seconds)


def terminate(process: subprocess.Popen[object], name: str) -> None:
    """Stop a child process, escalating only when it does not exit promptly."""
    if process.poll() is not None:
        return

    process.terminate()
    try:
        process.wait(timeout=5)
    except subprocess.TimeoutExpired:
        print(f"[warn] {name} did not exit after SIGTERM; sending SIGKILL", file=sys.stderr)
        process.kill()
        process.wait()


def wait_for_xvfb(process: subprocess.Popen[object], display: str) -> None:
    display_number = display.removeprefix(":").split(".", maxsplit=1)[0]
    socket_path = Path("/tmp/.X11-unix") / f"X{display_number}"
    deadline = time.monotonic() + 10
    while time.monotonic() < deadline:
        if process.poll() is not None:
            raise RuntimeError(f"Xvfb exited with code {process.returncode}")
        if socket_path.exists():
            return
        time.sleep(0.1)
    raise RuntimeError(f"Xvfb did not create display {display} within 10 seconds")


def wait_for_game(
    process: subprocess.Popen[object], seconds: float, executable_name: str, statistics: RunStatistics
) -> None:
    deadline = time.monotonic() + seconds
    while time.monotonic() < deadline:
        record_process_usage(process, statistics)
        if process.poll() is not None:
            raise RuntimeError(
                f"{executable_name} exited before the {seconds:g}-second screenshot delay "
                f"(exit code {process.returncode})"
            )
        time.sleep(min(0.1, deadline - time.monotonic()))


def capture_game_ui(
    executable: Path, screenshot: Path, warm_up_seconds: float, statistics: RunStatistics
) -> None:
    if not executable.is_file():
        raise FileNotFoundError(f"game executable not found: {executable}")

    environment = os.environ.copy()
    environment["DISPLAY"] = DISPLAY
    environment.setdefault("LIBGL_ALWAYS_SOFTWARE", "1")
    # Xvfb's Mesa software renderer provides a 4.5 core context on the CI image.
    environment["NEXIUM_HEADLESS_GL"] = "1"
    runtime_dir = Path("/tmp/nexium-xdg-runtime")
    runtime_dir.mkdir(mode=0o700, exist_ok=True)
    runtime_dir.chmod(0o700)
    environment["XDG_RUNTIME_DIR"] = str(runtime_dir)
    screenshot.unlink(missing_ok=True)
    screen_width, screen_height = get_screen_size()

    xvfb = subprocess.Popen(
        [
            "Xvfb",
            DISPLAY,
            "-screen",
            "0",
            f"{screen_width}x{screen_height}x24",
            "+extension",
            "GLX",
            "+iglx",
            "-nolisten",
            "tcp",
        ],
        stdout=subprocess.DEVNULL,
        stderr=subprocess.DEVNULL,
    )
    game: subprocess.Popen[object] | None = None
    try:
        wait_for_xvfb(xvfb, DISPLAY)
        game = subprocess.Popen(
            [str(executable.resolve())],
            cwd=Path.cwd(),
            env=environment,
        )
        statistics.started_at = time.monotonic()
        wait_for_game(game, warm_up_seconds, executable.name, statistics)

        screenshot_result = subprocess.run(
            ["scrot", str(screenshot)],
            env=environment,
            capture_output=True,
            text=True,
        )
        if screenshot_result.returncode != 0:
            output = (screenshot_result.stderr or screenshot_result.stdout).strip()
            raise RuntimeError(f"scrot failed with exit code {screenshot_result.returncode}: {output}")
        if not screenshot.is_file() or screenshot.stat().st_size == 0:
            raise RuntimeError("scrot completed without producing a PNG screenshot")
        record_process_usage(game, statistics)
        if game.poll() is not None:
            raise RuntimeError(
                f"{executable.name} exited while the UI screenshot was captured "
                f"(exit code {game.returncode})"
            )
    finally:
        if game is not None:
            record_process_usage(game, statistics)
            finish_statistics(statistics)
            terminate(game, executable.name)
        terminate(xvfb, "Xvfb")


def make_review_body(
    executable_name: str,
    tool_versions: list[tuple[str, str]],
    *,
    build_configuration: CmakeBuildConfiguration | None,
    image_url: str | None,
    screenshot_error: str | None,
    screen_size: tuple[int, int],
    statistics: RunStatistics,
) -> str:
    """Build the PR review body for both successful and failed captures."""
    lines = [f"**Test run of _{markdown_inline(executable_name)}_**"]
    if image_url:
        lines.extend(("", f"![{markdown_inline(executable_name)} UI]({image_url})"))
    elif screenshot_error:
        lines.extend(("", f"> Screenshot unavailable: `{markdown_inline(screenshot_error)}`"))

    if build_configuration is None:
        configuration_lines = ("CMake cache unavailable for this executable.",)
    else:
        configuration_lines = (
            "Reconstructed from the screenshot build's CMake cache and generated build files:",
            "```sh",
            build_configuration.configure_command,
            "```",
            "| Setting | Value |",
            "| --- | --- |",
            *(
                f"| {markdown_inline(name)} | `{markdown_inline(value)}` |"
                for name, value in build_configuration.details
            ),
        )

    lines.extend(
        (
            "",
            "### Build Environment",
            "| Tool | Version |",
            "| --- | --- |",
            *(f"| `{name}` | `{markdown_inline(version)}` |" for name, version in tool_versions),
            "",
            "### Build Configuration",
            *configuration_lines,
            "",
            "### Run Statistics",
            "| Metric | Value |",
            "| --- | --- |",
            f"| Virtual display | `{screen_size[0]}x{screen_size[1]}` via Xvfb |",
            f"| Elapsed time | `{statistics.elapsed_seconds:.2f} s` |"
            if statistics.elapsed_seconds is not None
            else "| Elapsed time | unavailable |",
            f"| Process CPU time | `{statistics.cpu_seconds:.2f} s` |"
            if statistics.cpu_seconds is not None
            else "| Process CPU time | unavailable |",
            f"| Average CPU utilization | "
            f"`{statistics.cpu_seconds / statistics.elapsed_seconds * 100:.1f}%` |"
            if statistics.cpu_seconds is not None
            and statistics.elapsed_seconds
            and statistics.elapsed_seconds > 0
            else "| Average CPU utilization | unavailable |",
            f"| Peak resident memory | `{format_bytes(statistics.peak_rss_bytes)}` |",
        )
    )
    return "\n".join(lines)


def publish_screenshot(
    client: GiteaClient,
    screenshot: Path,
    executable_name: str,
    tool_versions: list[tuple[str, str]],
    build_configuration: CmakeBuildConfiguration | None,
    screenshot_error: str | None,
    statistics: RunStatistics,
) -> None:
    pr_number = GiteaClient.resolve_pr_number()
    sha = GiteaClient.resolve_sha() or ""
    check_context = f"{executable_key(executable_name)}-ui"
    attachment_prefix = f"ci-{check_context}"
    check_state = "success" if screenshot_error is None else "failure"
    check_description = (
        f"{executable_name} UI screenshot captured"
        if screenshot_error is None
        else f"{executable_name} UI capture failed: {markdown_inline(screenshot_error)}"
    )
    if pr_number is None:
        print("[gitea] not a pull_request event - skipping screenshot publication")
        if sha:
            client.publish_check(
                sha,
                check_state,
                check_context,
                check_description,
            )
        return

    marker = review_marker(check_context)
    try:
        client.dismiss_previous_reviews(pr_number, marker=marker)
    except Exception as error:
        print(f"[gitea] failed to remove the previous screenshot review: {error}", file=sys.stderr)

    try:
        removed = client.delete_issue_attachments(pr_number, name_prefix=attachment_prefix)
        if removed:
            print(f"[gitea] removed {removed} stale screenshot attachment(s)")
    except Exception as error:
        print(f"[gitea] failed to remove stale screenshot attachments: {error}", file=sys.stderr)

    image_url = None
    if screenshot_error is None:
        try:
            attachment = client.upload_issue_attachment(
                pr_number, str(screenshot), name=f"{attachment_prefix}.png"
            )
            image_url = attachment["browser_download_url"]
        except Exception as error:
            screenshot_error = f"{executable_name} screenshot upload failed: {error}"
            check_state = "failure"
            check_description = f"{executable_name} UI screenshot upload failed"
            print(f"[gitea] {screenshot_error}", file=sys.stderr)

    review_body = make_review_body(
        executable_name,
        tool_versions,
        build_configuration=build_configuration,
        image_url=image_url,
        screenshot_error=screenshot_error,
        screen_size=get_screen_size(),
        statistics=statistics,
    )
    client.create_review(
        pr_number,
        body=review_body,
        event="COMMENT",
        commit_id=sha,
        marker=marker,
    )
    if sha:
        client.publish_check(
            sha,
            check_state,
            check_context,
            check_description,
        )


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--executable",
        default="build/bin/TemplateGame",
        help="game executable to run",
    )
    parser.add_argument(
        "--output",
        default=DEFAULT_SCREENSHOT_NAME,
        help="PNG screenshot output path",
    )
    parser.add_argument(
        "--warm-up-seconds",
        type=float,
        default=WARM_UP_SECONDS,
        help="seconds the game must remain alive before capture",
    )
    parser.add_argument(
        "--no-gitea",
        action="store_true",
        help="capture the screenshot without publishing to Gitea",
    )
    args = parser.parse_args()

    if args.warm_up_seconds <= 0:
        parser.error("--warm-up-seconds must be greater than zero")

    screenshot = Path(args.output).resolve()
    executable = Path(args.executable)
    tool_versions = collect_build_tool_versions()
    build_configuration = collect_cmake_build_configuration(executable)
    statistics = RunStatistics()
    screenshot_error = None
    try:
        capture_game_ui(executable, screenshot, args.warm_up_seconds, statistics)
    except (FileNotFoundError, RuntimeError, OSError, subprocess.SubprocessError) as error:
        screenshot_error = str(error)
        print(f"[error] {screenshot_error}", file=sys.stderr)

    if screenshot_error is None:
        print(f"Captured {executable.name} UI screenshot: {screenshot}")
    if args.no_gitea:
        if screenshot_error is not None:
            sys.exit(1)
        return

    client = GiteaClient.from_env()
    if client is not None:
        try:
            publish_screenshot(
                client,
                screenshot,
                executable.name,
                tool_versions,
                build_configuration,
                screenshot_error,
                statistics,
            )
        except Exception as error:
            print(f"[error] failed to publish {executable.name} UI screenshot: {error}", file=sys.stderr)
            sys.exit(1)

    if screenshot_error is not None:
        sys.exit(1)


if __name__ == "__main__":
    main()
