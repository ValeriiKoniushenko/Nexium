#!/usr/bin/env python3
"""Publish a gcovr summary, report attachment, and Gitea commit status."""

from __future__ import annotations

import argparse
import html
import json
import sys
import zipfile
from pathlib import Path

from gitea_client import GiteaClient, review_marker


CHECK_CONTEXT = "code-coverage"
ATTACHMENT_PREFIX = "ci-code-coverage"
METRICS = ("line", "function", "branch")
METRIC_LABELS = {"line": "lines", "function": "functions", "branch": "branches"}


def read_summary(path: Path) -> dict[str, object]:
    try:
        summary = json.loads(path.read_text())
    except (OSError, json.JSONDecodeError) as error:
        raise RuntimeError(f"could not read coverage summary '{path}': {error}") from error

    required = [
        f"{metric}_{field}"
        for metric in METRICS
        for field in ("covered", "total", "percent")
    ]
    missing = [key for key in required if key not in summary]
    if missing:
        raise RuntimeError(f"coverage summary is missing: {', '.join(missing)}")
    return summary


def percent(summary: dict[str, object], metric: str) -> str:
    value = summary[f"{metric}_percent"]
    return "n/a" if value is None else f"{float(value):.1f}%"


def ratio(summary: dict[str, object], metric: str) -> str:
    return f"{int(summary[f'{metric}_covered'])}/{int(summary[f'{metric}_total'])}"


def review_body(summary: dict[str, object], report_url: str | None) -> str:
    rows = "\n".join(
        f"| {METRIC_LABELS[metric].title()} | {percent(summary, metric)} | {ratio(summary, metric)} |"
        for metric in METRICS
    )
    download = (
        f'[Download the full HTML report]({html.escape(report_url, quote=True)}).'
        if report_url
        else "The report attachment could not be uploaded; see the CI job output."
    )
    return (
        "## Unit-test coverage\n\n"
        "| Metric | Coverage | Covered / total |\n"
        "| --- | ---: | ---: |\n"
        f"{rows}\n\n{download}"
    )


def archive_report(report: Path) -> Path:
    archive = report.with_name("code-coverage-report.zip")
    with zipfile.ZipFile(archive, "w", compression=zipfile.ZIP_DEFLATED) as output:
        output.write(report, arcname="index.html")
    return archive


def publish(summary: dict[str, object], report: Path, *, dry_run: bool) -> None:
    client = GiteaClient.from_env()
    if client is None:
        raise RuntimeError("Gitea client is not configured")

    sha = GiteaClient.resolve_sha() or ""
    pr_number = GiteaClient.resolve_pr_number()
    description = f"{percent(summary, 'line')} line coverage"
    if dry_run:
        print(f"[dry-run] would publish {description}")
        return
    if pr_number is None:
        if sha:
            client.publish_check(sha, "success", CHECK_CONTEXT, description)
        return

    marker = review_marker(CHECK_CONTEXT)
    client.dismiss_previous_reviews(pr_number, marker=marker)
    client.delete_issue_attachments(pr_number, name_prefix=ATTACHMENT_PREFIX)
    archive = archive_report(report)
    attachment = client.upload_issue_attachment(
        pr_number,
        str(archive),
        name=f"{ATTACHMENT_PREFIX}-{sha[:12] or 'latest'}.zip",
        content_type="application/zip",
    )
    report_url = str(attachment["browser_download_url"])
    client.create_review(
        pr_number,
        body=review_body(summary, report_url),
        commit_id=sha,
        marker=marker,
    )
    if sha:
        client.publish_check(
            sha,
            "success",
            CHECK_CONTEXT,
            description,
            target_url=report_url,
        )


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--summary", type=Path, required=True)
    parser.add_argument("--report", type=Path, required=True)
    parser.add_argument("--dry-run", action="store_true")
    parser.add_argument("--no-gitea", action="store_true")
    args = parser.parse_args()

    try:
        summary = read_summary(args.summary)
        if not args.report.is_file():
            raise RuntimeError(f"coverage report does not exist: {args.report}")
        print(
            "Code coverage: "
            + "; ".join(
                f"{METRIC_LABELS[metric]} {percent(summary, metric)} ({ratio(summary, metric)})"
                for metric in METRICS
            )
        )
        if not args.no_gitea:
            publish(summary, args.report, dry_run=args.dry_run)
    except RuntimeError as error:
        print(f"[coverage] {error}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
