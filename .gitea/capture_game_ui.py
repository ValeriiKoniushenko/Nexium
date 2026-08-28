#!/usr/bin/env python3
"""Capture TemplateGame's UI and publish it to the pull request that ran CI."""

from __future__ import annotations

import argparse
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
    screenshot.unlink(missing_ok=True)

    xvfb = subprocess.Popen(
        ["Xvfb", DISPLAY, "-screen", "0", "1920x1080x24", "+extension", "GLX", "-nolisten", "tcp"],
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


def publish_screenshot(client: GiteaClient, screenshot: Path) -> None:
    pr_number = GiteaClient.resolve_pr_number()
    sha = GiteaClient.resolve_sha() or ""
    if pr_number is None:
        print("[gitea] not a pull_request event - skipping screenshot publication")
        if sha:
            client.publish_check(
                sha,
                "success",
                "template-game-ui",
                "TemplateGame UI screenshot captured",
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

    attachment = client.upload_issue_attachment(pr_number, str(screenshot), name=SCREENSHOT_NAME)
    image_url = attachment["browser_download_url"]
    review_body = f"**TemplateGame UI**\n\n![TemplateGame UI]({image_url})"
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
            "success",
            "template-game-ui",
            "TemplateGame UI screenshot captured",
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
    try:
        capture_game_ui(Path(args.executable), screenshot, args.warm_up_seconds)
    except (FileNotFoundError, RuntimeError, OSError, subprocess.SubprocessError) as error:
        print(f"[error] {error}", file=sys.stderr)
        sys.exit(1)

    print(f"Captured TemplateGame UI screenshot: {screenshot}")
    if args.no_gitea:
        return

    client = GiteaClient.from_env()
    if client is not None:
        try:
            publish_screenshot(client, screenshot)
        except Exception as error:
            print(f"[error] failed to publish TemplateGame UI screenshot: {error}", file=sys.stderr)
            sys.exit(1)


if __name__ == "__main__":
    main()
