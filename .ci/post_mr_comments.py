#!/usr/bin/env python3
"""
Posts issues (from a gl-code-quality-report.json style file) as inline
discussion threads on a GitLab merge request diff, via the Discussions API.

This exists because GitLab CE does not render inline diff annotations from
the `codequality` artifacts report type -- that report only drives the
summary widget. Real inline comments require creating MR discussions with
a "position" payload pointing at the diff.

Docs: https://docs.gitlab.com/ee/api/discussions.html#create-new-merge-request-thread

CI usage (inside a merge_request_event pipeline):
    python3 ci/post_mr_comments.py --report gl-code-quality-report.json

Local/debug usage from your machine (dry-run, no network calls):
    python3 ci/post_mr_comments.py --report gl-code-quality-report.json \
        --project-id 42 --mr-iid 7 --token glpat-xxxx \
        --gitlab-url https://gitlab.vakon.dev \
        --base-sha <sha> --start-sha <sha> --head-sha <sha> \
        --dry-run --verbose

Required inputs (each can come from a CI_* env var or an explicit flag):
    --gitlab-url    CI_SERVER_URL
    --project-id    CI_PROJECT_ID
    --mr-iid        CI_MERGE_REQUEST_IID
    --base-sha      CI_MERGE_REQUEST_DIFF_BASE_SHA
    --start-sha     CI_MERGE_REQUEST_DIFF_START_SHA (falls back to base-sha if unset)
    --head-sha      CI_COMMIT_SHA
    --token         GITLAB_MR_COMMENT_TOKEN (a project/personal access token with api scope)
"""
import argparse
import json
import os
import sys
import urllib.parse
import urllib.request
import urllib.error


def env_or(flag_value: str | None, env_name: str) -> str | None:
    return flag_value or os.environ.get(env_name)


def load_issues(report_path: str) -> list[dict]:
    with open(report_path) as f:
        return json.load(f)


def already_posted_fingerprints(gitlab_url: str, project_id: str, mr_iid: str, token: str, verbose: bool) -> set[str]:
    """Fetch existing discussion notes so we don't repost the same issue on every pipeline run."""
    url = f"{gitlab_url}/api/v4/projects/{project_id}/merge_requests/{mr_iid}/discussions?per_page=100"
    seen = set()
    page = 1
    while url:
        req = urllib.request.Request(url, headers={"PRIVATE-TOKEN": token})
        try:
            with urllib.request.urlopen(req) as resp:
                discussions = json.loads(resp.read())
                link_header = resp.headers.get("Link", "")
        except urllib.error.HTTPError as e:
            if verbose:
                print(f"[debug] could not fetch existing discussions (page {page}): {e}", file=sys.stderr)
            break

        for d in discussions:
            for note in d.get("notes", []):
                body = note.get("body", "")
                # our comments embed a hidden marker: <!-- clang-tidy-fp:HASH -->
                if "<!-- clang-tidy-fp:" in body:
                    fp = body.split("<!-- clang-tidy-fp:")[1].split("-->")[0].strip()
                    seen.add(fp)

        # crude next-page detection from the Link header
        url = None
        for part in link_header.split(","):
            if 'rel="next"' in part:
                url = part.split(";")[0].strip().strip("<>")
                page += 1

    if verbose:
        print(f"[debug] {len(seen)} issue(s) already have a posted comment, will skip those")
    return seen


def post_discussion(gitlab_url: str, project_id: str, mr_iid: str, token: str,
                    base_sha: str, start_sha: str, head_sha: str,
                    path: str, line: int, body: str, verbose: bool, dry_run: bool) -> bool:
    payload = {
        "body": body,
        "position[position_type]": "text",
        "position[base_sha]": base_sha,
        "position[start_sha]": start_sha,
        "position[head_sha]": head_sha,
        "position[new_path]": path,
        "position[old_path]": path,
        "position[new_line]": str(line),
    }

    if dry_run:
        print(f"[dry-run] would POST discussion on {path}:{line}")
        if verbose:
            print(f"[dry-run] payload: {json.dumps(payload, indent=2)}")
        return True

    url = f"{gitlab_url}/api/v4/projects/{project_id}/merge_requests/{mr_iid}/discussions"
    data = urllib.parse.urlencode(payload).encode()
    req = urllib.request.Request(url, data=data, method="POST", headers={"PRIVATE-TOKEN": token})

    try:
        with urllib.request.urlopen(req) as resp:
            if verbose:
                print(f"[debug] posted comment on {path}:{line} -> HTTP {resp.status}")
            return True
    except urllib.error.HTTPError as e:
        err_body = e.read().decode(errors="replace")
        print(f"[warn] failed to post comment on {path}:{line}: HTTP {e.code} {err_body[:500]}", file=sys.stderr)
        return False


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--report", default="gl-code-quality-report.json",
                        help="codequality-style JSON report to read issues from")
    parser.add_argument("--gitlab-url")
    parser.add_argument("--project-id")
    parser.add_argument("--mr-iid")
    parser.add_argument("--base-sha")
    parser.add_argument("--start-sha")
    parser.add_argument("--head-sha")
    parser.add_argument("--token", help="API token; falls back to GITLAB_MR_COMMENT_TOKEN env var")
    parser.add_argument("--min-severity", choices=["minor", "major", "critical"], default="major",
                        help="only post comments at or above this severity (default: major)")
    parser.add_argument("--skip-duplicates", action="store_true", default=True,
                        help="skip issues that already have a posted comment, based on fingerprint (default: on)")
    parser.add_argument("--no-skip-duplicates", dest="skip_duplicates", action="store_false")
    parser.add_argument("--dry-run", action="store_true", help="print what would be posted without calling the API")
    parser.add_argument("--verbose", "-v", action="store_true")
    args = parser.parse_args()

    gitlab_url = env_or(args.gitlab_url, "CI_SERVER_URL")
    project_id = env_or(args.project_id, "CI_PROJECT_ID")
    mr_iid = env_or(args.mr_iid, "CI_MERGE_REQUEST_IID")
    base_sha = env_or(args.base_sha, "CI_MERGE_REQUEST_DIFF_BASE_SHA")
    start_sha = env_or(args.start_sha, "CI_MERGE_REQUEST_DIFF_START_SHA") or base_sha
    head_sha = env_or(args.head_sha, "CI_COMMIT_SHA")
    token = env_or(args.token, "GITLAB_MR_COMMENT_TOKEN")

    missing = [name for name, val in [
        ("gitlab-url", gitlab_url), ("project-id", project_id), ("mr-iid", mr_iid),
        ("base-sha", base_sha), ("head-sha", head_sha),
    ] if not val]
    if missing:
        print(f"[error] missing required values: {', '.join(missing)} "
              f"(pass as flags or ensure CI_MERGE_REQUEST_* vars are set)", file=sys.stderr)
        sys.exit(2)

    if not token and not args.dry_run:
        print("[error] no API token found (--token or GITLAB_MR_COMMENT_TOKEN). "
              "Use --dry-run to test without one.", file=sys.stderr)
        sys.exit(2)

    if not os.path.isfile(args.report):
        print(f"[error] report file not found: {args.report}", file=sys.stderr)
        sys.exit(2)

    issues = load_issues(args.report)
    if args.verbose:
        print(f"[debug] loaded {len(issues)} issue(s) from {args.report}")

    severity_rank = {"minor": 0, "major": 1, "critical": 2}
    min_rank = severity_rank[args.min_severity]

    already_posted = set()
    if args.skip_duplicates and not args.dry_run and token:
        already_posted = already_posted_fingerprints(gitlab_url, project_id, mr_iid, token, args.verbose)

    posted, skipped, failed = 0, 0, 0

    for issue in issues:
        severity = issue.get("severity", "major")
        if severity_rank.get(severity, 1) < min_rank:
            skipped += 1
            continue

        fp = issue.get("fingerprint", "")
        if fp in already_posted:
            if args.verbose:
                print(f"[debug] skipping already-posted issue: {fp}")
            skipped += 1
            continue

        path = issue["location"]["path"]
        line = issue["location"]["lines"]["begin"]
        description = issue["description"]

        body = (
            f"**clang-tidy**: {description}\n\n"
            f"<!-- clang-tidy-fp:{fp} -->"
        )

        ok = post_discussion(
            gitlab_url, project_id, mr_iid, token,
            base_sha, start_sha, head_sha,
            path, line, body, args.verbose, args.dry_run
        )
        if ok:
            posted += 1
        else:
            failed += 1

    print(f"Done. posted={posted} skipped={skipped} failed={failed}")
    if failed and not args.dry_run:
        sys.exit(1)


if __name__ == "__main__":
    main()
