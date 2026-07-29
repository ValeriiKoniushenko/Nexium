#!/usr/bin/env python3
"""Minimal Gitea API client for CI pull-request reviews and commit checks."""

from __future__ import annotations

import json
import os
import sys
import urllib.error
import urllib.request
from typing import Any


# Embedded in review bodies so we can find and remove our own reviews later.
REVIEW_MARKER = "<!-- ci:clang-format -->"


class GiteaClient:
    """Thin wrapper around the Gitea REST API used by CI scripts."""

    def __init__(
        self,
        server: str,
        token: str,
        owner: str,
        repo: str,
        *,
        verbose: bool = False,
        auth_scheme: str | None = None,
        token_source: str = "unknown",
    ) -> None:
        self.server = server.rstrip("/")
        self.token = token.strip()
        self.owner = owner
        self.repo = repo
        self.verbose = verbose
        self.token_source = token_source
        # Personal access tokens use "token"; OAuth2 / Actions task tokens use "Bearer".
        self.auth_scheme = (auth_scheme or os.environ.get("GITEA_AUTH_SCHEME") or "").strip()
        self._pending_comments: list[dict[str, Any]] = []
        self._user_id: int | None = None
        self._auth_resolved = False

    @classmethod
    def from_env(cls, verbose: bool = False) -> GiteaClient | None:
        """Build a client from Gitea Actions environment variables.

        Returns None when required variables are missing (e.g. local runs).
        """
        token = None
        token_source = None
        for key in ("GITEATOKEN", "GITHUB_TOKEN"):
            value = os.environ.get(key)
            if value and value.strip():
                token = value
                token_source = key
                break

        repo = (
            os.environ.get("GITEA_REPOSITORY")
            or os.environ.get("GITHUB_REPOSITORY")
        )
        server = (
            os.environ.get("GITEA_SERVER_URL")
            or os.environ.get("GITHUB_SERVER_URL")
        )

        if not all([token, repo, server]):
            if verbose:
                print(
                    "[gitea] missing GITEATOKEN / GITEA_REPOSITORY / "
                    "GITEA_SERVER_URL — skipping API calls",
                    file=sys.stderr,
                )
            return None

        if "/" not in repo:
            if verbose:
                print(f"[gitea] invalid repository value: {repo!r}", file=sys.stderr)
            return None

        owner, name = repo.split("/", 1)
        client = cls(
            server,
            token,
            owner,
            name,
            verbose=verbose,
            token_source=token_source or "unknown",
        )
        if verbose:
            print(
                f"[gitea] server={client.server} repo={owner}/{name} "
                f"token_source={client.token_source} token_len={len(client.token)}"
            )
        return client

    @staticmethod
    def resolve_pr_number() -> int | None:
        """Read the pull-request index from the Actions event payload."""
        event_path = (
            os.environ.get("GITEA_EVENT_PATH")
            or os.environ.get("GITHUB_EVENT_PATH")
        )
        if not event_path or not os.path.isfile(event_path):
            return None

        with open(event_path) as f:
            event = json.load(f)

        number = event.get("number")
        if number is None:
            number = event.get("pull_request", {}).get("number")
        return int(number) if number is not None else None

    @staticmethod
    def resolve_sha() -> str | None:
        return os.environ.get("GITEA_SHA") or os.environ.get("GITHUB_SHA")

    # ------------------------------------------------------------------ API

    def _auth_header(self, scheme: str) -> str:
        return f"{scheme} {self.token}"

    def _candidate_schemes(self) -> list[str]:
        if self.auth_scheme:
            return [self.auth_scheme]
        # Prefer Bearer for the automatic Actions token; PATs use "token".
        if self.token_source == "GITHUB_TOKEN":
            return ["Bearer", "token"]
        return ["token", "Bearer"]

    def _do_request(
        self,
        method: str,
        path: str,
        payload: dict[str, Any] | None,
        scheme: str,
    ) -> Any:
        url = f"{self.server}/api/v1{path}"
        data = None if payload is None else json.dumps(payload).encode()
        request = urllib.request.Request(
            url,
            data=data,
            method=method,
            headers={
                "Authorization": self._auth_header(scheme),
                "Content-Type": "application/json",
                "Accept": "application/json",
            },
        )
        with urllib.request.urlopen(request) as response:
            body = response.read()
            if self.verbose:
                print(f"[gitea] {method} {path} -> HTTP {response.status}")
            if not body or response.status == 204:
                return None
            return json.loads(body)

    def _resolve_auth(self) -> None:
        """Probe GET /user once so we know which Authorization scheme works."""
        if self._auth_resolved:
            return

        schemes = self._candidate_schemes()
        last_error: urllib.error.HTTPError | None = None

        for scheme in schemes:
            try:
                user = self._do_request("GET", "/user", None, scheme)
                self.auth_scheme = scheme
                self._auth_resolved = True
                if user and "id" in user:
                    self._user_id = int(user["id"])
                if self.verbose:
                    login = (user or {}).get("login", "?")
                    print(f"[gitea] authenticated as {login!r} via {scheme!r}")
                return
            except urllib.error.HTTPError as e:
                last_error = e
                if e.code != 401:
                    err = e.read().decode(errors="replace")
                    print(
                        f"[gitea] GET /user failed: HTTP {e.code} {err[:500]}",
                        file=sys.stderr,
                    )
                    raise
                if self.verbose:
                    print(
                        f"[gitea] auth scheme {scheme!r} rejected (401), trying next…",
                        file=sys.stderr,
                    )

        print(
            "[gitea] authentication failed (HTTP 401).\n"
            f"  token_source={self.token_source} token_len={len(self.token)}\n"
            "  Check that secrets.GITEATOKEN is a valid Personal Access Token\n"
            "  (Settings → Applications) with repository write scope, or set\n"
            "  GITEATOKEN to ${{ gitea.token }} / ${{ github.token }} in the workflow.\n"
            "  Verify with:\n"
            f"    curl -H 'Authorization: token <PAT>' {self.server}/api/v1/user",
            file=sys.stderr,
        )
        if last_error is not None:
            raise last_error
        raise RuntimeError("gitea authentication failed")

    def _request(
        self,
        method: str,
        path: str,
        payload: dict[str, Any] | None = None,
    ) -> Any:
        self._resolve_auth()
        assert self.auth_scheme
        try:
            return self._do_request(method, path, payload, self.auth_scheme)
        except urllib.error.HTTPError as e:
            err = e.read().decode(errors="replace")
            print(
                f"[gitea] {method} {path} failed: HTTP {e.code} {err[:500]}",
                file=sys.stderr,
            )
            raise

    def _current_user_id(self) -> int | None:
        if self._user_id is not None:
            return self._user_id
        try:
            user = self._request("GET", "/user")
        except urllib.error.HTTPError:
            return None
        self._user_id = int(user["id"])
        return self._user_id

    def publish_check(
        self,
        sha: str,
        state: str,
        context: str,
        description: str,
        *,
        target_url: str = "",
    ) -> None:
        """Create a commit status (shown next to the commit / PR checks).

        ``state`` must be one of: pending, success, error, failure.
        """
        payload: dict[str, Any] = {
            "state": state,
            "context": context,
            "description": description[:140],
        }
        if target_url:
            payload["target_url"] = target_url

        self._request(
            "POST",
            f"/repos/{self.owner}/{self.repo}/statuses/{sha}",
            payload,
        )

    def add_review_comment(
        self,
        path: str,
        body: str,
        *,
        new_position: int,
        old_position: int = 0,
    ) -> dict[str, Any]:
        """Queue an inline (Files Changed) review comment.

        Comments are submitted together by :meth:`create_review`.
        ``new_position`` is the line number in the new file version.
        """
        comment: dict[str, Any] = {
            "path": path,
            "body": body,
            "new_position": new_position,
            "old_position": old_position,
        }
        self._pending_comments.append(comment)
        return comment

    def create_review(
        self,
        pr_number: int,
        body: str,
        *,
        event: str = "COMMENT",
        commit_id: str = "",
        comments: list[dict[str, Any]] | None = None,
    ) -> dict[str, Any] | None:
        """Submit a pull-request review, optionally with inline comments.

        If ``comments`` is omitted, any comments previously queued via
        :meth:`add_review_comment` are used (and then cleared).
        """
        if comments is None:
            comments = list(self._pending_comments)
            self._pending_comments.clear()

        if not comments and not body.strip():
            if self.verbose:
                print("[gitea] create_review: nothing to submit")
            return None

        marked_body = body
        if REVIEW_MARKER not in marked_body:
            marked_body = f"{body.rstrip()}\n\n{REVIEW_MARKER}"

        payload: dict[str, Any] = {
            "body": marked_body,
            "event": event,
            "comments": comments,
        }
        if commit_id:
            payload["commit_id"] = commit_id

        return self._request(
            "POST",
            f"/repos/{self.owner}/{self.repo}/pulls/{pr_number}/reviews",
            payload,
        )

    def dismiss_previous_reviews(
        self,
        pr_number: int,
        *,
        marker: str = REVIEW_MARKER,
    ) -> int:
        """Delete previous CI reviews (and their inline comments) for this PR.

        Gitea has no "update review comments" API that replaces a prior run's
        annotations, so we delete matching reviews owned by the token user.
        Returns the number of reviews removed.
        """
        try:
            reviews = self._request(
                "GET",
                f"/repos/{self.owner}/{self.repo}/pulls/{pr_number}/reviews",
            )
        except urllib.error.HTTPError:
            return 0

        if not reviews:
            return 0

        user_id = self._current_user_id()
        removed = 0

        for review in reviews:
            if review.get("dismissed"):
                continue

            body = review.get("body") or ""
            if marker not in body:
                continue

            reviewer = review.get("user") or {}
            if user_id is not None and reviewer.get("id") != user_id:
                continue

            review_id = review["id"]
            try:
                self._request(
                    "DELETE",
                    f"/repos/{self.owner}/{self.repo}/pulls/{pr_number}/reviews/{review_id}",
                )
                removed += 1
            except urllib.error.HTTPError:
                # Fall back to dismiss if delete is forbidden.
                try:
                    self._request(
                        "POST",
                        f"/repos/{self.owner}/{self.repo}/pulls/{pr_number}"
                        f"/reviews/{review_id}/dismissals",
                        {"message": "Superseded by a newer CI run", "priors": False},
                    )
                    removed += 1
                except urllib.error.HTTPError:
                    pass

        if self.verbose:
            print(f"[gitea] dismissed/removed {removed} previous review(s)")
        return removed
