from __future__ import annotations

import json
import os
import sys
import tempfile
import unittest
from pathlib import Path
from unittest.mock import patch

from cpp_ci import clang_format
from cpp_ci.reports import write_json_report


class ReportTests(unittest.TestCase):
    def test_creates_missing_parent_directory(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            report_path = Path(directory) / "ci-results" / "clang-format.json"

            write_json_report(str(report_path), [{"check_name": "clang-format"}])

            self.assertEqual(
                json.loads(report_path.read_text(encoding="utf-8")),
                [{"check_name": "clang-format"}],
            )

    def test_formatter_creates_its_report_directory(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            previous_directory = os.getcwd()
            try:
                os.chdir(directory)
                with (
                    patch("cpp_ci.clang_format.validate_clang_format_config"),
                    patch("cpp_ci.clang_format.get_target_branch", return_value="develop"),
                    patch("cpp_ci.clang_format.get_changed_files", return_value=[]),
                    patch.object(
                        sys,
                        "argv",
                        [
                            "clang_format.py",
                            "--report-path",
                            "ci-results/clang-format.json",
                            "--no-gitea",
                        ],
                    ),
                ):
                    clang_format.main()
            finally:
                os.chdir(previous_directory)

            self.assertEqual(
                json.loads(
                    (Path(directory) / "ci-results" / "clang-format.json").read_text(
                        encoding="utf-8"
                    )
                ),
                [],
            )


if __name__ == "__main__":
    unittest.main()
