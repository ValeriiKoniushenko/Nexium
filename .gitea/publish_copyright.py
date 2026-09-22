#!/usr/bin/env python3
"""Publish copyright results as a Gitea PR review and commit status."""

import argparse
import json
import sys

from gitea_client import GiteaClient, review_marker


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--report-path", default="copyright-report.json")
    parser.add_argument("--dry-run", action="store_true")
    args = parser.parse_args()

    try:
        with open(args.report_path) as report:
            issues = json.load(report)["issues"]
    except (OSError, KeyError, TypeError, json.JSONDecodeError) as error:
        print(f"[copyright] cannot read report: {error}", file=sys.stderr)
        return 1

    marker = review_marker("copyright")
    pr_number = GiteaClient.resolve_pr_number()
    sha = GiteaClient.resolve_sha() or ""
    if args.dry_run:
        print(f"[dry-run] would publish {len(issues)} copyright violation(s)")
        return 0

    client = GiteaClient.from_env()
    if client is None:
        print("[copyright] Gitea client is not configured", file=sys.stderr)
        return 1

    try:
        if pr_number is not None:
            client.dismiss_previous_reviews(pr_number, marker=marker)
            if issues:
                details = "\n".join(
                    f"- `{issue['path']}:{issue['line']}` — {issue['message']}"
                    for issue in issues
                )
                client.create_review(
                    pr_number,
                    body=(
                        f"Copyright check found {len(issues)} violation(s).\n\n"
                        f"{details}"
                    ),
                    marker=marker,
                )

        if sha:
            client.publish_check(
                sha,
                "failure" if issues else "success",
                "copyright",
                (
                    f"{len(issues)} copyright violation(s)"
                    if issues
                    else "copyright clean"
                ),
            )
    except Exception as error:
        print(f"[copyright] failed to publish results: {error}", file=sys.stderr)
        return 1

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
