import sys
import unittest
from pathlib import Path


sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import check_valgrind  # noqa: E402
import run_game_valgrind  # noqa: E402
from gitea_client import classified_review_body  # noqa: E402


class ReviewClassificationTests(unittest.TestCase):
    def test_classification_banners_are_first(self):
        expected = {
            "error": "❗ **! ERROR",
            "warning": "⚠️ **! WARNING",
            "info": "ℹ️ **! INFO",
        }
        for classification, prefix in expected.items():
            with self.subTest(classification=classification):
                self.assertTrue(
                    classified_review_body("details", classification).startswith(prefix)
                )

    def test_unknown_classification_is_rejected(self):
        with self.assertRaises(ValueError):
            classified_review_body("details", "unknown")

    def test_unit_test_valgrind_failure_starts_with_error(self):
        result = check_valgrind.ValgrindResult(["valgrind"], 42, "", "failure")
        self.assertTrue(check_valgrind.failure_review_body(result).startswith("❗ **! ERROR"))

    def test_game_valgrind_failure_starts_with_error(self):
        result = run_game_valgrind.ValgrindGameResult(
            ["valgrind"], 42, "", "ERROR SUMMARY: 1 errors", 5
        )
        self.assertTrue(
            run_game_valgrind.review_body("TemplateGame", result).startswith(
                "❗ **! ERROR"
            )
        )


if __name__ == "__main__":
    unittest.main()
