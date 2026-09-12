#!/usr/bin/env python3
"""Check that each provided file starts with exactly one copyright header."""

from __future__ import annotations

import argparse
from datetime import date
import re
import sys
from pathlib import Path


COPYRIGHT_START_YEAR = 2018
COPYRIGHT_HOLDER = "Valerii Koniushenko"
CURRENT_YEAR = date.today().year
COPYRIGHT_RE = re.compile(rb"\bcopyright\b", re.IGNORECASE)
COPYRIGHT_LINE = (
    f"// Copyright {COPYRIGHT_START_YEAR}-{CURRENT_YEAR} "
    f"{COPYRIGHT_HOLDER}\n"
).encode("ascii")
EXPECTED_LINES = (
    COPYRIGHT_LINE,
    b"//\n",
    b'// Licensed under the Apache License, Version 2.0 (the "License");\n',
    b"// you may not use this file except in compliance with the License.\n",
    b"// You may obtain a copy of the License at\n",
    b"//\n",
    b"//     http://www.apache.org/licenses/LICENSE-2.0\n",
)
PROJECT_LINE_RE = re.compile(rb"// [^\r\n]+\n")
COPYRIGHT_LINE_RE = re.compile(
    rb"// Copyright 2018-(\d{4}) Valerii Koniushenko(?:\r\n|\n)?"
)


def check_file(path: Path) -> list[str]:
    try:
        content = path.read_bytes()
    except OSError as error:
        return [f"{path}: cannot read file: {error}"]

    copyright_matches = list(COPYRIGHT_RE.finditer(content))
    if not copyright_matches:
        return [f"{path}: copyright notice is missing"]

    if len(copyright_matches) != 1:
        line_numbers = [
            content.count(b"\n", 0, match.start()) + 1
            for match in copyright_matches
        ]
        return [
            f"{path}: expected exactly one copyright notice, found "
            f"{len(copyright_matches)} at lines "
            + ", ".join(map(str, line_numbers))
        ]

    lines = content.splitlines(keepends=True)
    match = copyright_matches[0]
    copyright_index = content.count(b"\n", 0, match.start())
    actual_line = lines[copyright_index]
    issues: list[str] = []

    if copyright_index != 1:
        issues.append(
            f"{path}:{copyright_index + 1}: copyright line must be line 2; "
            "the header must start at byte 0 on line 1"
        )

    year_match = COPYRIGHT_LINE_RE.fullmatch(actual_line)
    if year_match is not None:
        actual_year = int(year_match.group(1))
        if actual_year != CURRENT_YEAR:
            issues.append(
                f"{path}:{copyright_index + 1}: invalid copyright year "
                f"{actual_year}; expected {CURRENT_YEAR}"
            )

    if issues:
        return issues

    project_offset = lines[0].find(b"// ")
    if project_offset > 0:
        return [
            f"{path}: copyright header starts at byte {project_offset}; "
            "expected byte 0"
        ]

    if PROJECT_LINE_RE.fullmatch(lines[0]) is None:
        return [
            f"{path}:1: header text mismatch; expected b'// <PROJECT_NAME>\\n', "
            f"found {lines[0]!r}"
        ]

    for line_number, expected in enumerate(EXPECTED_LINES, start=2):
        if len(lines) < line_number:
            return [
                f"{path}: header is incomplete at line {line_number}; "
                f"expected {expected!r}"
            ]

        actual = lines[line_number - 1]
        if actual != expected:
            return [
                f"{path}:{line_number}: header text mismatch; "
                f"expected {expected!r}, found {actual!r}"
            ]

    return []


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
