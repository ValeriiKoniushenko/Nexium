#!/usr/bin/env python3
"""Remove this workflow's previous pull-request reviews before a new run."""

from __future__ import annotations

from .gitea_client import GiteaClient, review_marker

def main(
        *,
        review_contexts: tuple[str, ...],
        attachment_prefixes: tuple[str, ...] = (),
) -> None:
    client = GiteaClient.from_env()
    if client is None:
        print("[gitea] client not configured - skipping CI review cleanup")
        return

    pr_number = GiteaClient.resolve_pr_number()
    if pr_number is None:
        print("[gitea] not a pull_request event - skipping CI review cleanup")
        return

    removed_reviews = 0
    for context in review_contexts:
        removed_reviews += client.dismiss_previous_reviews(
            pr_number,
            marker=review_marker(context),
        )

    removed_attachments = sum(
        client.delete_issue_attachments(pr_number, name_prefix=prefix)
        for prefix in attachment_prefixes
    )
    print(
        f"[gitea] removed {removed_reviews} previous CI review(s) and "
        f"{removed_attachments} coverage attachment(s)"
    )
