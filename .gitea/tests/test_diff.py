from __future__ import annotations

import subprocess
import unittest
from unittest.mock import patch

from cpp_ci.diff import ChangedFile, DiffError, get_changed_files, get_cpp_analysis_targets, prepare_base_ref


class DiffTests(unittest.TestCase):
    def test_explicit_files_honor_extensions_and_exclusions(self) -> None:
        changed = get_changed_files(
            "develop",
            ["sources/Thing.cpp", "docs/Guide.hpp", "assets/logo.png", "tests/Test.hxx"],
            verbose=False,
            excluded_paths=("docs/",),
            extensions=(".cpp", ".hxx"),
        )
        self.assertEqual([item.path for item in changed], ["sources/Thing.cpp", "tests/Test.hxx"])

    def test_changed_header_uses_existing_sibling_source_when_available(self) -> None:
        with patch("cpp_ci.diff.os.path.isfile", side_effect=lambda value: value == "sources/Thing.cpp"):
            targets = get_cpp_analysis_targets([ChangedFile("sources/Thing.hpp", [])])
        self.assertEqual(targets, ["sources/Thing.cpp"])

    def test_base_fetch_failure_is_not_silently_treated_as_an_empty_diff(self) -> None:
        failed = subprocess.CompletedProcess(["git"], 128, "", "network unavailable")
        with patch("cpp_ci.diff.subprocess.run", return_value=failed):
            with self.assertRaisesRegex(DiffError, "cannot fetch base branch"):
                prepare_base_ref("develop")


if __name__ == "__main__":
    unittest.main()
