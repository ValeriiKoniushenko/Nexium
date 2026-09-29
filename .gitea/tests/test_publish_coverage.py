import contextlib
import io
import json
import sys
import tempfile
import unittest
import zipfile
from pathlib import Path
from unittest.mock import Mock


sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import publish_coverage  # noqa: E402
from gitea_client import _token_preview  # noqa: E402


SUMMARY = {
    "line_covered": 90,
    "line_total": 100,
    "line_percent": 90.0,
    "function_covered": 8,
    "function_total": 10,
    "function_percent": 80.0,
    "branch_covered": 7,
    "branch_total": 10,
    "branch_percent": 70.0,
}


class PublishCoverageTests(unittest.TestCase):
    def test_reads_gcovr_summary(self):
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "summary.json"
            path.write_text(json.dumps(SUMMARY))
            self.assertEqual(publish_coverage.read_summary(path), SUMMARY)

    def test_review_contains_all_totals_and_report_link(self):
        body = publish_coverage.review_body(SUMMARY, "https://gitea/report.zip")
        self.assertTrue(body.startswith("ℹ️ **! INFO"))
        self.assertIn("90.0%", body)
        self.assertIn("8/10", body)
        self.assertIn("7/10", body)
        self.assertIn("https://gitea/report.zip", body)

    def test_archives_report_as_index(self):
        with tempfile.TemporaryDirectory() as directory:
            report = Path(directory) / "report.html"
            report.write_text("coverage")
            archive = publish_coverage.archive_report(report)
            with zipfile.ZipFile(archive) as content:
                self.assertEqual(content.read("index.html"), b"coverage")

    def test_commit_status_permission_failure_is_nonfatal(self):
        client = Mock()
        client.publish_check.side_effect = PermissionError("forbidden")
        with contextlib.redirect_stderr(io.StringIO()) as errors:
            publish_coverage.publish_optional_status(client, "abc", "90% coverage")
        self.assertIn("optional commit status", errors.getvalue())

    def test_token_preview_does_not_expose_token_characters(self):
        self.assertEqual(_token_preview("secret-token"), "<redacted; 12 chars>")


if __name__ == "__main__":
    unittest.main()
