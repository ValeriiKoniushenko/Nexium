#!/usr/bin/env python3
"""Run the project copyright checker on changed C/C++ files."""

import argparse
import subprocess
import sys

from utils import get_changed_files, get_target_branch


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--base", help="branch/ref to diff against")
    parser.add_argument("--files", nargs="+", help="explicit files; skips git diff")
    parser.add_argument("--verbose", "-v", action="store_true")
    args = parser.parse_args()

    changed = get_changed_files(
        get_target_branch(args.base),
        args.files,
        args.verbose,
    )
    files = [file.path for file in changed]
    if not files:
        print("No changed C/C++ files require copyright checks.")
        return 0

    return subprocess.run(
        [sys.executable, "scripts/check_copyright.py", *files],
        check=False,
    ).returncode


if __name__ == "__main__":
    raise SystemExit(main())
