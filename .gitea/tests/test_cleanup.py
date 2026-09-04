from __future__ import annotations

import unittest
from unittest.mock import Mock, patch

from cpp_ci import cleanup


class CleanupTests(unittest.TestCase):
    def test_removes_only_the_requested_ci_review_contexts(self) -> None:
        client = Mock()
        client.dismiss_previous_reviews.side_effect = (2, 0)
        client.delete_issue_attachments.return_value = 1

        with (
            patch.object(cleanup.GiteaClient, "from_env", return_value=client),
            patch.object(cleanup.GiteaClient, "resolve_pr_number", return_value=42),
        ):
            cleanup.main(
                review_contexts=("clang-format", "clang-tidy"),
                attachment_prefixes=("ci-code-coverage",),
            )

        self.assertEqual(client.dismiss_previous_reviews.call_count, 2)
        self.assertEqual(client.dismiss_previous_reviews.call_args_list[0].args, (42,))
        self.assertEqual(
            client.dismiss_previous_reviews.call_args_list[0].kwargs,
            {"marker": "<!-- ci:clang-format -->"},
        )
        client.delete_issue_attachments.assert_called_once_with(
            42, name_prefix="ci-code-coverage"
        )

    def test_skips_cleanup_outside_a_pull_request(self) -> None:
        client = Mock()

        with (
            patch.object(cleanup.GiteaClient, "from_env", return_value=client),
            patch.object(cleanup.GiteaClient, "resolve_pr_number", return_value=None),
        ):
            cleanup.main(review_contexts=("clang-format",))

        client.dismiss_previous_reviews.assert_not_called()


if __name__ == "__main__":
    unittest.main()
