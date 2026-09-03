#!/usr/bin/env python3
"""Aggregate unit-test reports into one replaceable Gitea pull-request review."""

from __future__ import annotations

import argparse
import json
import os
import sys
from dataclasses import dataclass, field
from pathlib import Path

from .gitea_client import GiteaClient, review_marker
from .unit_tests import (
    LOCATION_RE,
    FailureLocation,
    UnitTestResult,
    failure_blocks_by_test,
    parse_result,
)


REPORT_CONTEXT = "unit-tests"
MAX_INFRASTRUCTURE_OUTPUT_CHARS = 2_000


@dataclass
class VariantReport:
    label: str
    result: UnitTestResult


@dataclass
class AggregatedFailure:
    name: str
    variants: list[str] = field(default_factory=list)
    locations: list[FailureLocation] = field(default_factory=list)
    bodies: list[str] = field(default_factory=list)


@dataclass
class AggregateResult:
    reports: list[VariantReport]
    missing_labels: list[str]
    failures: list[AggregatedFailure]

    @property
    def failed(self) -> bool:
        return bool(self.missing_labels or any(report.result.failed for report in self.reports))

    @property
    def check_state(self) -> str:
        if self.missing_labels or any(
                report.result.infrastructure_error for report in self.reports
        ):
            return "error"
        return "failure" if self.failed else "success"


def load_reports(report_dir: str) -> list[VariantReport]:
    reports = []
    seen_labels = set()
    for report_path in sorted(Path(report_dir).rglob("*.json")):
        with open(report_path) as report_file:
            payload = json.load(report_file)

        if payload.get("schema_version") != 1:
            raise ValueError(f"unsupported report schema in {report_path}")

        label = str(payload["label"])
        if label in seen_labels:
            raise ValueError(f"duplicate unit-test report for {label}")
        seen_labels.add(label)

        command = [str(item) for item in payload.get("command", [])]
        output = str(payload.get("output", ""))
        returncode = int(payload["returncode"])
        reports.append(VariantReport(label, parse_result(command, returncode, output)))
    return reports


def aggregate_reports(
        reports: list[VariantReport],
        expected_labels: list[str],
) -> AggregateResult:
    by_label = {report.label: report for report in reports}
    ordered_reports = [by_label[label] for label in expected_labels if label in by_label]
    ordered_reports.extend(
        report for report in reports if report.label not in expected_labels
    )
    missing_labels = [label for label in expected_labels if label not in by_label]

    by_test: dict[str, AggregatedFailure] = {}
    for report in ordered_reports:
        blocks = failure_blocks_by_test(report.result.output)
        for test_name in report.result.failed_tests:
            failure = by_test.setdefault(test_name, AggregatedFailure(test_name))
            if report.label not in failure.variants:
                failure.variants.append(report.label)
            for body in blocks.get(test_name, []):
                for match in LOCATION_RE.finditer(body):
                    location = FailureLocation(
                        match.group("path"),
                        int(match.group("line")),
                    )
                    if location not in failure.locations:
                        failure.locations.append(location)
                if body not in failure.bodies:
                    failure.bodies.append(body)

            if len(report.result.failed_tests) == 1 and not failure.locations:
                for location in report.result.locations:
                    if location not in failure.locations:
                        failure.locations.append(location)

    return AggregateResult(ordered_reports, missing_labels, list(by_test.values()))


def result_description(result: AggregateResult) -> str:
    if not result.failed:
        return f"Unit tests passed in {len(result.reports)} build variants"
    if result.missing_labels:
        return f"Unit-test reports missing for {len(result.missing_labels)} build variants"
    return f"Unit tests failed: {len(result.failures)} unique failed test(s)"


def _failure_section(failure: AggregatedFailure) -> str:
    variants = ", ".join(failure.variants)
    lines = [f"### `{failure.name}`", "", f"Failed in: {variants}"]
    if failure.locations:
        locations = ", ".join(
            f"`{location.path}:{location.line}`" for location in failure.locations
        )
        lines.extend(("", f"Location: {locations}"))

    if failure.bodies:
        for body in failure.bodies:
            sanitized = body.replace("```", "''' ")
            lines.extend(("", "```text", sanitized, "```"))
    else:
        lines.extend(("", "No GoogleTest assertion body was captured."))
    return "\n".join(lines)


def _infrastructure_sections(result: AggregateResult) -> list[str]:
    sections = []
    for report in result.reports:
        if not report.result.failed or report.result.failed_tests:
            continue
        output = report.result.output.strip().replace("```", "''' ")
        if len(output) > MAX_INFRASTRUCTURE_OUTPUT_CHARS:
            output = output[-MAX_INFRASTRUCTURE_OUTPUT_CHARS:]
        sections.append(
            f"### {report.label}\n\n"
            f"Test process exited with code `{report.result.returncode}` before reporting "
            "a failed test.\n\n"
            f"```text\n{output or 'No output captured.'}\n```"
        )
    return sections


def failure_review_body(result: AggregateResult) -> str:
    sections = ["**Unit tests failed**"]
    if result.missing_labels:
        missing = ", ".join(result.missing_labels)
        sections.append(f"Missing reports: {missing}")
    sections.extend(_failure_section(failure) for failure in result.failures)
    sections.extend(_infrastructure_sections(result))
    return "\n\n".join(sections)


def publish_result(
        client: GiteaClient,
        result: AggregateResult,
        *,
        dry_run: bool,
        verbose: bool,
) -> None:
    pr_number = GiteaClient.resolve_pr_number()
    sha = GiteaClient.resolve_sha() or ""
    marker = review_marker(REPORT_CONTEXT)
    description = result_description(result)

    if pr_number is None:
        if verbose:
            print("[gitea] not a pull_request event - skipping aggregate unit-test review")
        if sha and not dry_run:
            try:
                client.publish_check(sha, result.check_state, REPORT_CONTEXT, description)
            except Exception as error:
                print(f"[gitea] failed to publish unit-test check: {error}", file=sys.stderr)
        return

    if dry_run:
        print(f"[dry-run] would replace aggregate unit-test review on PR #{pr_number}")
        if result.failed:
            print(failure_review_body(result))
        return

    try:
        client.dismiss_previous_reviews(pr_number, marker=marker)
    except Exception as error:
        print(f"[gitea] failed to clear previous unit-test review: {error}", file=sys.stderr)

    if result.failed:
        try:
            client.create_review(
                pr_number,
                body=failure_review_body(result),
                event="COMMENT",
                commit_id=sha,
                marker=marker,
            )
        except Exception as error:
            print(f"[gitea] failed to create unit-test review: {error}", file=sys.stderr)

    if sha:
        try:
            client.publish_check(sha, result.check_state, REPORT_CONTEXT, description)
        except Exception as error:
            print(f"[gitea] failed to publish unit-test check: {error}", file=sys.stderr)


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--report-dir", required=True)
    parser.add_argument("--expected", action="append", default=[])
    parser.add_argument("--dry-run", action="store_true")
    parser.add_argument("--no-gitea", action="store_true")
    parser.add_argument("--verbose", "-v", action="store_true")
    args = parser.parse_args()

    try:
        reports = load_reports(args.report_dir)
        result = aggregate_reports(reports, args.expected)
    except (OSError, ValueError, KeyError, TypeError, json.JSONDecodeError) as error:
        print(f"[error] failed to aggregate unit-test reports: {error}", file=sys.stderr)
        sys.exit(2)

    if not args.no_gitea:
        client = GiteaClient.from_env(verbose=args.verbose)
        if client is not None:
            publish_result(client, result, dry_run=args.dry_run, verbose=args.verbose)
        elif args.verbose:
            print("[gitea] client not configured - skipping aggregate unit-test review")

    print(result_description(result))
    if result.failed:
        sys.exit(1)
