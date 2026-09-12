#!/usr/bin/env python3
"""Check that each provided file starts with exactly one Nexium copyright header."""

from __future__ import annotations

import argparse
from datetime import date
import re
import sys
from pathlib import Path


PROJECT_NAME = "Nexium"
COPYRIGHT_START_YEAR = 2018
COPYRIGHT_HOLDER = "Valerii Koniushenko"
COPYRIGHT_RE = re.compile(rb"\bcopyright\b", re.IGNORECASE)
EXPECTED_HEADER = (
    f"// {PROJECT_NAME}\n"
    f"// Copyright {COPYRIGHT_START_YEAR}-{date.today().year} {COPYRIGHT_HOLDER}\n"
    "//\n"
    '// Licensed under the Apache License, Version 2.0 (the "License");\n'
    "// you may not use this file except in compliance with the License.\n"
    "// You may obtain a copy of the License at\n"
    "//\n"
    "//     http://www.apache.org/licenses/LICENSE-2.0\n"
).encode("ascii")


def check_file(path: Path) -> list[str]:
    try:
        content = path.read_bytes()
    except OSError as error:
        return [f"{path}: cannot read file: {error}"]

    copyright_count = len(COPYRIGHT_RE.findall(content))
    issues = []

    if copyright_count != 1:
        issues.append(
            f"{path}: expected exactly one copyright notice, "
            f"found {copyright_count}"
        )

    if not content.startswith(EXPECTED_HEADER):
        issues.append(
            f"{path}: the copyright header must exactly match the expected header "
            "at byte 0"
        )

    return issues


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("files", nargs="+", type=Path, help="files to check")
    args = parser.parse_args()

    issues: list[str] = []
    for path in args.files:
        if not path.is_file():
            issues.append(f"{path}: file does not exist")
            continue
        issues.extend(check_file(path))

    if issues:
        for issue in issues:
            print(issue, file=sys.stderr)
        return 1

    print(f"Checked {len(args.files)} file(s): copyright notices are valid.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
