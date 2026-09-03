"""Generic headless application capture and Valgrind primitives.

Nothing in this module knows a project executable, window cache, or renderer.
Projects pass a command, optional window title, and environment variables from
their own CI configuration/wrapper.
"""

from __future__ import annotations

import argparse
import os
import re
import shutil
import subprocess
import sys
import time
from pathlib import Path


DISPLAY = ":99"
VALGRIND_ERROR_EXIT_CODE = 42
ERROR_SUMMARY_RE = re.compile(r"ERROR SUMMARY: (?P<count>\d+) errors?")


def _parse_screen_size(value: str) -> tuple[int, int]:
    match = re.fullmatch(r"(\d+)x(\d+)", value)
    if not match or int(match.group(1)) < 1 or int(match.group(2)) < 1:
        raise argparse.ArgumentTypeError("screen size must be WIDTHxHEIGHT with positive integers")
    return int(match.group(1)), int(match.group(2))


def _parse_environment(values: list[str]) -> dict[str, str]:
    environment = os.environ.copy()
    for value in values:
        name, separator, content = value.partition("=")
        if not separator or not name:
            raise ValueError(f"environment value must use NAME=VALUE syntax: {value!r}")
        environment[name] = content
    return environment


def _application_command(values: list[str]) -> list[str]:
    """Return an argparse remainder without its optional ``--`` separator."""
    command = list(values)
    if command[:1] == ["--"]:
        command.pop(0)
    if not command:
        raise ValueError("an application command is required after '--'")
    return command


def _terminate(process: subprocess.Popen[object], label: str) -> None:
    if process.poll() is not None:
        return
    process.terminate()
    try:
        process.wait(timeout=5)
    except subprocess.TimeoutExpired:
        print(f"[runtime] {label} ignored SIGTERM; sending SIGKILL", file=sys.stderr)
        process.kill()
        process.wait()


def _wait_for_xvfb(process: subprocess.Popen[object], display: str) -> None:
    display_number = display.removeprefix(":").split(".", maxsplit=1)[0]
    socket = Path("/tmp/.X11-unix") / f"X{display_number}"
    deadline = time.monotonic() + 10
    while time.monotonic() < deadline:
        if process.poll() is not None:
            raise RuntimeError(f"Xvfb exited with code {process.returncode}")
        if socket.exists():
            return
        time.sleep(0.1)
    raise RuntimeError(f"Xvfb did not create display {display} within 10 seconds")


def _start_xvfb(display: str, screen_size: tuple[int, int]) -> subprocess.Popen[object]:
    if shutil.which("Xvfb") is None:
        raise RuntimeError("Xvfb is not installed in this CI image")
    width, height = screen_size
    process = subprocess.Popen(
        [
            "Xvfb",
            display,
            "-screen",
            "0",
            f"{width}x{height}x24",
            "+extension",
            "GLX",
            "+iglx",
            "-nolisten",
            "tcp",
        ],
        stdout=subprocess.DEVNULL,
        stderr=subprocess.DEVNULL,
    )
    _wait_for_xvfb(process, display)
    return process


def _headless_environment(display: str, overrides: list[str]) -> dict[str, str]:
    environment = _parse_environment(overrides)
    environment["DISPLAY"] = display
    environment.setdefault("LIBGL_ALWAYS_SOFTWARE", "1")
    runtime_dir = Path("/tmp/cpp-ci-xdg-runtime")
    runtime_dir.mkdir(mode=0o700, exist_ok=True)
    runtime_dir.chmod(0o700)
    environment.setdefault("XDG_RUNTIME_DIR", str(runtime_dir))
    return environment


def _wait_for_application(process: subprocess.Popen[object], seconds: float) -> None:
    deadline = time.monotonic() + seconds
    while time.monotonic() < deadline:
        if process.poll() is not None:
            raise RuntimeError(f"application exited before capture (exit code {process.returncode})")
        time.sleep(min(0.1, max(0.0, deadline - time.monotonic())))


def _capture(output: Path, *, window_title: str | None, environment: dict[str, str]) -> None:
    output.parent.mkdir(parents=True, exist_ok=True)
    output.unlink(missing_ok=True)
    if window_title:
        if shutil.which("xdotool") is None or shutil.which("import") is None:
            raise RuntimeError(
                "--window-title needs both xdotool and ImageMagick's import command; "
                "install xdotool imagemagick in the CI image, or omit --window-title for a root-window capture"
            )
        search = subprocess.run(
            ["xdotool", "search", "--name", window_title],
            env=environment,
            capture_output=True,
            text=True,
        )
        window_ids = search.stdout.split()
        if search.returncode != 0 or not window_ids:
            details = (search.stderr or search.stdout).strip() or "no matching X11 window"
            raise RuntimeError(f"cannot find window titled {window_title!r}: {details}")
        capture_command = ["import", "-window", window_ids[-1], str(output)]
    else:
        if shutil.which("scrot") is None:
            raise RuntimeError("scrot is not installed in this CI image")
        capture_command = ["scrot", str(output)]
    result = subprocess.run(capture_command, env=environment, capture_output=True, text=True)
    if result.returncode != 0 or not output.is_file() or output.stat().st_size == 0:
        details = (result.stderr or result.stdout).strip() or "no image produced"
        raise RuntimeError(f"screenshot command failed: {details}")


def capture_window(args: argparse.Namespace) -> int:
    command = _application_command(args.command)
    environment = _headless_environment(args.display, args.environment)
    output = Path(args.output).resolve()
    xvfb = _start_xvfb(args.display, args.screen_size)
    application: subprocess.Popen[object] | None = None
    try:
        print(f"[runtime] starting: {' '.join(command)}")
        application = subprocess.Popen(command, cwd=Path.cwd(), env=environment)
        _wait_for_application(application, args.warmup)
        _capture(output, window_title=args.window_title, environment=environment)
        if application.poll() is not None:
            raise RuntimeError(f"application exited during capture (exit code {application.returncode})")
        print(f"[runtime] captured {output}")
        return 0
    finally:
        if application is not None:
            _terminate(application, "application")
        _terminate(xvfb, "Xvfb")


def run_under_valgrind(args: argparse.Namespace) -> int:
    application_command = _application_command(args.command)
    valgrind = args.valgrind
    if shutil.which(valgrind) is None:
        raise RuntimeError(f"Valgrind command is unavailable: {valgrind}")
    if shutil.which("timeout") is None:
        raise RuntimeError("the GNU timeout command is unavailable in this CI image")
    suppression_arguments: list[str] = []
    for value in args.suppression:
        path = Path(value)
        if not path.is_file():
            raise RuntimeError(f"Valgrind suppression file does not exist: {path}")
        suppression_arguments.append(f"--suppressions={path}")
    valgrind_command = [
        valgrind,
        "--leak-check=full",
        "--show-leak-kinds=definite",
        "--errors-for-leak-kinds=definite",
        "--track-origins=yes",
        f"--error-exitcode={VALGRIND_ERROR_EXIT_CODE}",
        *suppression_arguments,
    ]
    if args.generate_suppressions:
        valgrind_command.append("--gen-suppressions=all")
    valgrind_command.extend(application_command)
    command = [
        "timeout",
        "--signal=TERM",
        "--kill-after=5s",
        f"{args.timeout:g}s",
        *valgrind_command,
    ]
    environment = _headless_environment(args.display, args.environment) if args.headless else _parse_environment(args.environment)
    xvfb = _start_xvfb(args.display, args.screen_size) if args.headless else None
    try:
        print(f"+ {' '.join(command)}")
        completed = subprocess.run(command, cwd=Path.cwd(), env=environment, capture_output=True, text=True)
    finally:
        if xvfb is not None:
            _terminate(xvfb, "Xvfb")

    if completed.stdout:
        print(completed.stdout, end="" if completed.stdout.endswith("\n") else "\n")
    if completed.stderr:
        print(completed.stderr, file=sys.stderr, end="" if completed.stderr.endswith("\n") else "\n")
    matches = list(ERROR_SUMMARY_RE.finditer(completed.stderr))
    errors = int(matches[-1].group("count")) if matches else None
    if errors is None:
        print("[runtime] Valgrind did not produce an ERROR SUMMARY", file=sys.stderr)
        return 1
    if errors:
        print(f"[runtime] Valgrind found {errors} error(s)", file=sys.stderr)
        return 1
    if args.expect_timeout:
        if completed.returncode == 124:
            print(f"[runtime] completed cleanly until the expected {args.timeout:g}s timeout")
            return 0
        print(
            f"[runtime] application exited before the expected {args.timeout:g}s timeout "
            f"(exit code {completed.returncode})",
            file=sys.stderr,
        )
        return 1
    if completed.returncode != 0:
        print(f"[runtime] application/Valgrind exited with code {completed.returncode}", file=sys.stderr)
        return completed.returncode
    print("[runtime] Valgrind clean")
    return 0


def _parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description=__doc__)
    commands = parser.add_subparsers(dest="action", required=True)
    capture = commands.add_parser("capture-window", help="capture an application under Xvfb")
    capture.add_argument("--output", default="application-ui.png")
    capture.add_argument("--window-title", help="optional X11 window title to capture instead of the root window")
    capture.add_argument("--warmup", type=float, default=5.0)
    capture.add_argument("--display", default=DISPLAY)
    capture.add_argument("--screen-size", type=_parse_screen_size, default=(1920, 1080))
    capture.add_argument("--environment", action="append", default=[], metavar="NAME=VALUE")
    capture.add_argument("command", nargs=argparse.REMAINDER)
    valgrind = commands.add_parser("run-under-valgrind", help="run an arbitrary application under Valgrind")
    valgrind.add_argument("--valgrind", default="valgrind")
    valgrind.add_argument("--timeout", type=float, default=5.0)
    valgrind.add_argument("--expect-timeout", action="store_true")
    valgrind.add_argument("--headless", action="store_true")
    valgrind.add_argument("--display", default=DISPLAY)
    valgrind.add_argument("--screen-size", type=_parse_screen_size, default=(1920, 1080))
    valgrind.add_argument("--suppression", action="append", default=[])
    valgrind.add_argument("--generate-suppressions", action="store_true")
    valgrind.add_argument("--environment", action="append", default=[], metavar="NAME=VALUE")
    valgrind.add_argument("command", nargs=argparse.REMAINDER)
    return parser


def main() -> None:
    args = _parser().parse_args()
    if args.action == "capture-window" and args.warmup <= 0:
        raise SystemExit("--warmup must be greater than zero")
    if args.action == "run-under-valgrind" and args.timeout <= 0:
        raise SystemExit("--timeout must be greater than zero")
    try:
        status = capture_window(args) if args.action == "capture-window" else run_under_valgrind(args)
    except (OSError, RuntimeError, ValueError, subprocess.SubprocessError) as error:
        print(f"[runtime] {error}", file=sys.stderr)
        status = 1
    raise SystemExit(status)


if __name__ == "__main__":
    main()
