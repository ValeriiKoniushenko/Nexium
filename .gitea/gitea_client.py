#!/usr/bin/env python3
"""Minimal Gitea API client for CI pull-request reviews and commit checks."""

from __future__ import annotations

import json
import os
import sys
import urllib.error
import urllib.request
from typing import Any


# Embedded in review bodies so CI jobs can replace only their own annotations.
REVIEW_MARKER = "<!-- ci:clang-format -->"


def review_marker(check_name: str) -> str:
    """Return the marker used to identify reviews created by a CI check."""
    return f"<!-- ci:{check_name} -->"

# Env vars probed for the API token, in priority order.
_TOKEN_ENV_KEYS = (
    "GITEATOKEN",
    "GITEA_TOKEN",
    "GITHUB_TOKEN",
)


def _token_preview(token: str, *, head: int = 4, tail: int = 2) -> str:
    """Return a safe redacted preview of a secret token."""
    if not token:
        return "<empty>"
    if len(token) <= head + tail + 3:
        return f"{token[:1]}…({len(token)} chars)"
    return f"{token[:head]}…{token[-tail:]} (len={len(token)})"


def _looks_quoted(value: str) -> bool:
    return len(value) >= 2 and value[0] == value[-1] and value[0] in "'\""


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

        self._trace(
            f"client init: server={self.server!r} owner={self.owner!r} "
            f"repo={self.repo!r} token_source={self.token_source!r} "
            f"token={_token_preview(self.token)} "
            f"auth_scheme={self.auth_scheme or '<auto>'!r}"
        )
        self._warn_token_shape(token)

    # -------------------------------------------------------------- logging

    def _trace(self, message: str) -> None:
        print(f"[gitea] {message}", file=sys.stderr)

    def _debug(self, message: str) -> None:
        if self.verbose:
            print(f"[gitea:debug] {message}", file=sys.stderr)

    def _warn(self, message: str) -> None:
        print(f"[gitea:warn] {message}", file=sys.stderr)

    def _warn_token_shape(self, raw_token: str) -> None:
        """Emit warnings for common secret-misconfiguration mistakes."""
        if not raw_token:
            self._warn("token is empty")
            return

        if raw_token != raw_token.strip():
            self._warn(
                "token has leading/trailing whitespace "
                f"(raw_len={len(raw_token)}, stripped_len={len(raw_token.strip())})"
            )

        if "\n" in raw_token or "\r" in raw_token:
            self._warn("token contains newline/carriage-return characters")

        if _looks_quoted(raw_token.strip()):
            self._warn(
                "token appears wrapped in quotes — paste the raw token value "
                "into the secret, without surrounding quotes"
            )

        if " " in self.token:
            self._warn("stripped token still contains spaces (suspicious)")

        if len(self.token) < 20:
            self._warn(
                f"token looks unusually short ({len(self.token)} chars) — "
                "check that secrets.GITEATOKEN is set correctly"
            )

        # Actions / Gitea sometimes expose placeholder or literal template text.
        lowered = self.token.lower()
        for bad in ("your_token", "changeme", "***", "${{", "giteatoken"):
            if bad in lowered:
                self._warn(f"token contains suspicious substring {bad!r}")

    @classmethod
    def from_env(cls, verbose: bool = False) -> GiteaClient | None:
        """Build a client from Gitea Actions environment variables.

        Returns None when required variables are missing (e.g. local runs).
        """
        print("[gitea] resolving client from environment…", file=sys.stderr)

        # Show which token-related env keys are present (not their values).
        for key in _TOKEN_ENV_KEYS:
            raw = os.environ.get(key)
            if raw is None:
                print(f"[gitea]   env {key}: <unset>", file=sys.stderr)
            elif not raw.strip():
                print(
                    f"[gitea]   env {key}: <set but empty/whitespace "
                    f"raw_len={len(raw)}>",
                    file=sys.stderr,
                )
            else:
                print(
                    f"[gitea]   env {key}: set preview={_token_preview(raw.strip())}",
                    file=sys.stderr,
                )

        token = None
        token_source = None
        for key in _TOKEN_ENV_KEYS:
            value = os.environ.get(key)
            if value and value.strip():
                token = value
                token_source = key
                break

        repo_key = None
        repo = None
        for key in ("GITEA_REPOSITORY", "GITHUB_REPOSITORY"):
            value = os.environ.get(key)
            if value is None:
                print(f"[gitea]   env {key}: <unset>", file=sys.stderr)
            else:
                print(f"[gitea]   env {key}: {value!r}", file=sys.stderr)
                if value and not repo:
                    repo = value
                    repo_key = key

        server_key = None
        server = None
        for key in ("GITEA_SERVER_URL", "GITHUB_SERVER_URL"):
            value = os.environ.get(key)
            if value is None:
                print(f"[gitea]   env {key}: <unset>", file=sys.stderr)
            else:
                print(f"[gitea]   env {key}: {value!r}", file=sys.stderr)
                if value and not server:
                    server = value
                    server_key = key

        missing = [
            name
            for name, val in (
                ("token", token),
                ("repository", repo),
                ("server", server),
            )
            if not val
        ]
        if missing:
            print(
                f"[gitea:warn] missing required config: {', '.join(missing)} "
                "— skipping API calls",
                file=sys.stderr,
            )
            return None

        if "/" not in repo:
            print(
                f"[gitea:warn] invalid repository value from {repo_key}: {repo!r}",
                file=sys.stderr,
            )
            return None

        owner, name = repo.split("/", 1)
        print(
            f"[gitea] using token_source={token_source} "
            f"repo_source={repo_key} server_source={server_key}",
            file=sys.stderr,
        )
        return cls(
            server,
            token,
            owner,
            name,
            verbose=verbose,
            token_source=token_source or "unknown",
        )

    @staticmethod
    def resolve_pr_number() -> int | None:
        """Read the pull-request index from the Actions event payload."""
        event_path = (
            os.environ.get("GITEA_EVENT_PATH")
            or os.environ.get("GITHUB_EVENT_PATH")
        )
        if not event_path:
            print("[gitea] PR number: no GITEA/GITHUB_EVENT_PATH set", file=sys.stderr)
            return None
        if not os.path.isfile(event_path):
            print(
                f"[gitea:warn] PR number: event path does not exist: {event_path!r}",
                file=sys.stderr,
            )
            return None

        with open(event_path) as f:
            event = json.load(f)

        number = event.get("number")
        if number is None:
            number = event.get("pull_request", {}).get("number")

        event_name = (
            os.environ.get("GITEA_EVENT_NAME")
            or os.environ.get("GITHUB_EVENT_NAME")
            or event.get("action")
            or "?"
        )
        if number is None:
            print(
                f"[gitea] PR number: not found in event payload "
                f"(event_path={event_path!r} event_name={event_name!r} "
                f"top_keys={sorted(event.keys())})",
                file=sys.stderr,
            )
            return None

        print(
            f"[gitea] PR number={number} (event_path={event_path!r} "
            f"event_name={event_name!r})",
            file=sys.stderr,
        )
        return int(number)

    @staticmethod
    def resolve_sha() -> str | None:
        for key in ("GITEA_SHA", "GITHUB_SHA"):
            value = os.environ.get(key)
            if value:
                print(
                    f"[gitea] sha={value} (from {key})",
                    file=sys.stderr,
                )
                return value
        print("[gitea:warn] sha: GITEA_SHA / GITHUB_SHA unset", file=sys.stderr)
        return None

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
        self._trace(
            f"HTTP {method} {url} auth={scheme!r} "
            f"token={_token_preview(self.token)} "
            f"payload_bytes={0 if data is None else len(data)}"
        )
        if payload is not None:
            self._debug(f"payload keys={sorted(payload.keys())}")

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
            self._trace(
                f"HTTP {method} {path} -> {response.status} "
                f"body_bytes={len(body)}"
            )
            if not body or response.status == 204:
                return None
            return json.loads(body)

    def _resolve_auth(self) -> None:
        """Probe GET /user once so we know which Authorization scheme works."""
        if self._auth_resolved:
            self._debug(f"auth already resolved: scheme={self.auth_scheme!r}")
            return

        schemes = self._candidate_schemes()
        self._trace(
            f"probing auth schemes={schemes} "
            f"token_source={self.token_source!r} "
            f"token={_token_preview(self.token)}"
        )
        last_error: urllib.error.HTTPError | None = None

        for scheme in schemes:
            try:
                user = self._do_request("GET", "/user", None, scheme)
                self.auth_scheme = scheme
                self._auth_resolved = True
                if user and "id" in user:
                    self._user_id = int(user["id"])
                login = (user or {}).get("login", "?")
                email = (user or {}).get("email", "")
                self._trace(
                    f"authenticated as login={login!r} id={self._user_id} "
                    f"email={email!r} via scheme={scheme!r}"
                )
                return
            except urllib.error.HTTPError as e:
                last_error = e
                err_body = e.read().decode(errors="replace")
                self._warn(
                    f"auth scheme {scheme!r} failed: HTTP {e.code} "
                    f"body={err_body[:300]!r}"
                )
                if e.code != 401:
                    raise

        self._warn(
            "authentication failed (HTTP 401) for all schemes.\n"
            f"  token_source={self.token_source} "
            f"token={_token_preview(self.token)}\n"
            "  Check that secrets.GITEATOKEN is a valid Personal Access Token\n"
            "  (Settings → Applications) with repository write scope, or set\n"
            "  GITEA_TOKEN to ${{ gitea.token }} / ${{ github.token }} in the workflow.\n"
            "  Verify with:\n"
            f"    curl -H 'Authorization: token <PAT>' {self.server}/api/v1/user"
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
            self._warn(
                f"{method} {path} failed: HTTP {e.code} {err[:500]} "
                f"(auth={self.auth_scheme!r} token={_token_preview(self.token)})"
            )
            raise
        except urllib.error.URLError as e:
            self._warn(f"{method} {path} network error: {e}")
            raise

    def _current_user_id(self) -> int | None:
        if self._user_id is not None:
            return self._user_id
        try:
            user = self._request("GET", "/user")
        except (urllib.error.HTTPError, urllib.error.URLError):
            self._warn("could not resolve current user id")
            return None
        self._user_id = int(user["id"])
        self._trace(f"current user id={self._user_id}")
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

        self._trace(
            f"publish_check sha={sha[:12]}… state={state!r} "
            f"context={context!r} description={description!r}"
        )
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
        self._trace(
            f"queued review comment #{len(self._pending_comments)} "
            f"path={path!r} new_position={new_position} "
            f"body_len={len(body)}"
        )
        return comment

    def create_review(
        self,
        pr_number: int,
        body: str,
        *,
        event: str = "COMMENT",
        commit_id: str = "",
        comments: list[dict[str, Any]] | None = None,
        marker: str = REVIEW_MARKER,
    ) -> dict[str, Any] | None:
        """Submit a pull-request review, optionally with inline comments.

        If ``comments`` is omitted, any comments previously queued via
        :meth:`add_review_comment` are used (and then cleared).
        """
        if comments is None:
            comments = list(self._pending_comments)
            self._pending_comments.clear()

        if not comments and not body.strip():
            self._trace("create_review: nothing to submit")
            return None

        marked_body = body
        if marker and marker not in marked_body:
            marked_body = f"{body.rstrip()}\n\n{marker}"

        payload: dict[str, Any] = {
            "body": marked_body,
            "event": event,
            "comments": comments,
        }
        if commit_id:
            payload["commit_id"] = commit_id

        self._trace(
            f"create_review pr=#{pr_number} event={event!r} "
            f"commit_id={commit_id[:12] + '…' if commit_id else '<default>'} "
            f"comments={len(comments)} body_len={len(marked_body)}"
        )
        for i, c in enumerate(comments, start=1):
            self._trace(
                f"  comment[{i}] path={c.get('path')!r} "
                f"new_position={c.get('new_position')} "
                f"old_position={c.get('old_position')}"
            )

        result = self._request(
            "POST",
            f"/repos/{self.owner}/{self.repo}/pulls/{pr_number}/reviews",
            payload,
        )
        if isinstance(result, dict):
            self._trace(
                f"create_review ok id={result.get('id')} "
                f"state={result.get('state')!r}"
            )
        return result

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
        self._trace(f"dismiss_previous_reviews pr=#{pr_number} marker={marker!r}")
        try:
            reviews = self._request(
                "GET",
                f"/repos/{self.owner}/{self.repo}/pulls/{pr_number}/reviews",
            )
        except (urllib.error.HTTPError, urllib.error.URLError):
            self._warn("could not list reviews — skipping dismiss")
            return 0

        if not reviews:
            self._trace("no existing reviews on this PR")
            return 0

        self._trace(f"listed {len(reviews)} review(s)")
        user_id = self._current_user_id()
        removed = 0

        for review in reviews:
            review_id = review.get("id")
            reviewer = review.get("user") or {}
            body = review.get("body") or ""
            dismissed = bool(review.get("dismissed"))
            has_marker = marker in body

            self._debug(
                f"review id={review_id} reviewer_id={reviewer.get('id')} "
                f"login={reviewer.get('login')!r} dismissed={dismissed} "
                f"has_marker={has_marker} body_len={len(body)}"
            )

            if dismissed:
                continue
            if not has_marker:
                continue
            if user_id is not None and reviewer.get("id") != user_id:
                self._trace(
                    f"skip review id={review_id}: owned by "
                    f"{reviewer.get('login')!r}, not current user"
                )
                continue

            try:
                self._request(
                    "DELETE",
                    f"/repos/{self.owner}/{self.repo}/pulls/{pr_number}/reviews/{review_id}",
                )
                self._trace(f"deleted review id={review_id}")
                removed += 1
            except (urllib.error.HTTPError, urllib.error.URLError):
                # Fall back to dismiss if delete is forbidden.
                self._warn(f"delete review id={review_id} failed — trying dismiss")
                try:
                    self._request(
                        "POST",
                        f"/repos/{self.owner}/{self.repo}/pulls/{pr_number}"
                        f"/reviews/{review_id}/dismissals",
                        {"message": "Superseded by a newer CI run", "priors": False},
                    )
                    self._trace(f"dismissed review id={review_id}")
                    removed += 1
                except (urllib.error.HTTPError, urllib.error.URLError):
                    self._warn(f"could not delete or dismiss review id={review_id}")

        self._trace(f"dismissed/removed {removed} previous review(s)")
        return removed
