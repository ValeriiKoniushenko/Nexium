#!/usr/bin/env python3
"""Capture TemplateGame's UI and publish it to the pull request that ran CI."""

from __future__ import annotations

import argparse
import json
import os
import subprocess
import sys
import time
from pathlib import Path

from gitea_client import GiteaClient, review_marker


WARM_UP_SECONDS = 5.0
DISPLAY = ":99"
SCREENSHOT_NAME = "ci-template-game-ui.png"
SCREENSHOT_PREFIX = "ci-template-game-ui"
DEFAULT_SCREEN_SIZE = (1920, 1080)
WINDOW_CACHE_PATH = Path("data/cache/RootWindow.json")
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


def markdown_inline(value: str) -> str:
    """Keep command output and errors on one safe Markdown line."""
    return value.replace("`", "'").replace("\r", " ").replace("\n", " ")


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


def wait_for_game(process: subprocess.Popen[object], seconds: float) -> None:
    deadline = time.monotonic() + seconds
    while time.monotonic() < deadline:
        if process.poll() is not None:
            raise RuntimeError(
                f"TemplateGame exited before the {seconds:g}-second screenshot delay "
                f"(exit code {process.returncode})"
            )
        time.sleep(min(0.1, deadline - time.monotonic()))


def capture_game_ui(executable: Path, screenshot: Path, warm_up_seconds: float) -> None:
    if not executable.is_file():
        raise FileNotFoundError(f"TemplateGame executable not found: {executable}")

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
        wait_for_game(game, warm_up_seconds)

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
        if game.poll() is not None:
            raise RuntimeError(
                f"TemplateGame exited while the UI screenshot was captured "
                f"(exit code {game.returncode})"
            )
    finally:
        if game is not None:
            terminate(game, "TemplateGame")
        terminate(xvfb, "Xvfb")


def make_review_body(
    tool_versions: list[tuple[str, str]],
    *,
    image_url: str | None,
    screenshot_error: str | None,
    screen_size: tuple[int, int],
) -> str:
    """Build the PR review body for both successful and failed captures."""
    lines = ["**Test run of the _TemplateGame_**"]
    if image_url:
        lines.extend(("", f"![TemplateGame UI]({image_url})"))
    elif screenshot_error:
        lines.extend(("", f"> Screenshot unavailable: `{markdown_inline(screenshot_error)}`"))

    lines.extend(
        (
            "",
            "It was built with:",
            *(f"- `{name}`: `{markdown_inline(version)}`" for name, version in tool_versions),
            "",
            f"Virtual display: `{screen_size[0]}x{screen_size[1]}` via Xvfb.",
        )
    )
    return "\n".join(lines)


def publish_screenshot(
    client: GiteaClient,
    screenshot: Path,
    tool_versions: list[tuple[str, str]],
    screenshot_error: str | None,
) -> None:
    pr_number = GiteaClient.resolve_pr_number()
    sha = GiteaClient.resolve_sha() or ""
    check_state = "success" if screenshot_error is None else "failure"
    check_description = (
        "TemplateGame UI screenshot captured"
        if screenshot_error is None
        else f"TemplateGame UI capture failed: {markdown_inline(screenshot_error)}"
    )
    if pr_number is None:
        print("[gitea] not a pull_request event - skipping screenshot publication")
        if sha:
            client.publish_check(
                sha,
                check_state,
                "template-game-ui",
                check_description,
            )
        return

    marker = review_marker("template-game-ui")
    try:
        client.dismiss_previous_reviews(pr_number, marker=marker)
    except Exception as error:
        print(f"[gitea] failed to remove the previous screenshot review: {error}", file=sys.stderr)

    try:
        removed = client.delete_issue_attachments(pr_number, name_prefix=SCREENSHOT_PREFIX)
        if removed:
            print(f"[gitea] removed {removed} stale screenshot attachment(s)")
    except Exception as error:
        print(f"[gitea] failed to remove stale screenshot attachments: {error}", file=sys.stderr)

    image_url = None
    if screenshot_error is None:
        try:
            attachment = client.upload_issue_attachment(pr_number, str(screenshot), name=SCREENSHOT_NAME)
            image_url = attachment["browser_download_url"]
        except Exception as error:
            screenshot_error = f"Screenshot upload failed: {error}"
            check_state = "failure"
            check_description = "TemplateGame UI screenshot upload failed"
            print(f"[gitea] {screenshot_error}", file=sys.stderr)

    review_body = make_review_body(
        tool_versions,
        image_url=image_url,
        screenshot_error=screenshot_error,
        screen_size=get_screen_size(),
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
            "template-game-ui",
            check_description,
        )


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--executable",
        default="build/bin/TemplateGame",
        help="TemplateGame executable to run",
    )
    parser.add_argument(
        "--output",
        default=SCREENSHOT_NAME,
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
    tool_versions = collect_build_tool_versions()
    screenshot_error = None
    try:
        capture_game_ui(Path(args.executable), screenshot, args.warm_up_seconds)
    except (FileNotFoundError, RuntimeError, OSError, subprocess.SubprocessError) as error:
        screenshot_error = str(error)
        print(f"[error] {screenshot_error}", file=sys.stderr)

    if screenshot_error is None:
        print(f"Captured TemplateGame UI screenshot: {screenshot}")
    if args.no_gitea:
        if screenshot_error is not None:
            sys.exit(1)
        return

    client = GiteaClient.from_env()
    if client is not None:
        try:
            publish_screenshot(client, screenshot, tool_versions, screenshot_error)
        except Exception as error:
            print(f"[error] failed to publish TemplateGame UI screenshot: {error}", file=sys.stderr)
            sys.exit(1)

    if screenshot_error is not None:
        sys.exit(1)


if __name__ == "__main__":
    main()
