from __future__ import annotations

import subprocess
import unittest
from unittest.mock import Mock, patch

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

    def test_test_exit_does_not_create_a_valgrind_failure_review(self) -> None:
        result = valgrind.ValgrindResult(["valgrind", "Tests"], 1, "test failed", "")
        client = Mock()

        with (
            patch.object(valgrind.GiteaClient, "resolve_pr_number", return_value=42),
            patch.object(valgrind.GiteaClient, "resolve_sha", return_value="a" * 40),
        ):
            valgrind.publish_result(client, result, dry_run=False, verbose=False)

        self.assertFalse(result.valgrind_error)
        self.assertEqual(
            valgrind.result_description(result),
            "Test executable failed under Valgrind (exit code 1)",
        )
        client.create_review.assert_not_called()
        client.publish_check.assert_called_once_with(
            "a" * 40,
            "failure",
            "valgrind",
            "Test executable failed under Valgrind (exit code 1)",
        )

    def test_valgrind_error_exit_creates_a_failure_review(self) -> None:
        result = valgrind.ValgrindResult(
            ["valgrind", "Tests"],
            valgrind.VALGRIND_ERROR_EXIT_CODE,
            "",
            "ERROR SUMMARY: 1 errors",
        )
        client = Mock()

        with (
            patch.object(valgrind.GiteaClient, "resolve_pr_number", return_value=42),
            patch.object(valgrind.GiteaClient, "resolve_sha", return_value="a" * 40),
        ):
            valgrind.publish_result(client, result, dry_run=False, verbose=False)

        self.assertTrue(result.valgrind_error)
        client.create_review.assert_called_once()


if __name__ == "__main__":
    unittest.main()
