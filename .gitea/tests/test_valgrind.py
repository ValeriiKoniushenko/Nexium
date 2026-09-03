from __future__ import annotations

import subprocess
import unittest
from unittest.mock import patch

from cpp_ci import valgrind


class ValgrindTests(unittest.TestCase):
    def test_generated_suppression_blocks_and_all_configured_files_are_forwarded(self) -> None:
        completed = subprocess.CompletedProcess(["valgrind"], 0, "tests passed", "ERROR SUMMARY: 0 errors")
        with patch("cpp_ci.valgrind.subprocess.run", return_value=completed):
            result = valgrind.run_valgrind(
                "build/bin/Tests",
                ["--gtest_filter=Core.*"],
                ["valgrind.supp", "os.supp"],
                generate_suppressions=True,
            )
        self.assertIn("--gen-suppressions=all", result.command)
        self.assertIn("--suppressions=valgrind.supp", result.command)
        self.assertIn("--suppressions=os.supp", result.command)
        self.assertEqual(result.command[-2:], ["build/bin/Tests", "--gtest_filter=Core.*"])


if __name__ == "__main__":
    unittest.main()
