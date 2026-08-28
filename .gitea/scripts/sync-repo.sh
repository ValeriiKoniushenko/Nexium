#!/usr/bin/env bash
# Syncs the shared, bind-mounted /repo-cache to the current commit and
# copies it into $GITHUB_WORKSPACE. Falls back to a fresh shallow clone
# if no cache exists yet.
#
# Required env: GITEATOKEN, GITHUB_REPOSITORY, GITHUB_SHA, GITHUB_WORKSPACE
# Optional env: EXTRA_REFS (space-separated refs to fetch alongside the SHA,
#               default: empty), SHALLOW ("true" to add --depth=1 on the
#               incremental fetch against an existing cache, default: false)
set -euo pipefail

: "${GITEATOKEN:?GITEATOKEN is required}"
: "${GITHUB_REPOSITORY:?GITHUB_REPOSITORY is required}"
: "${GITHUB_SHA:?GITHUB_SHA is required}"
: "${GITHUB_WORKSPACE:?GITHUB_WORKSPACE is required}"

EXTRA_REFS="${EXTRA_REFS:-}"
SHALLOW="${SHALLOW:-false}"

echo "::group::Sync repo cache"

REPO_URL="https://oauth2:${GITEATOKEN}@gitea.vakon.dev/${GITHUB_REPOSITORY}.git"

DEPTH_FLAG=""
if [ "$SHALLOW" = "true" ]; then
  DEPTH_FLAG="--depth=1"
fi

if [ -d /repo-cache/.git ]; then
  echo "Using existing repo cache"
  cd /repo-cache
  git remote set-url origin "$REPO_URL"
  # shellcheck disable=SC2086
  git fetch $DEPTH_FLAG origin "$GITHUB_SHA" $EXTRA_REFS
  git checkout -f "$GITHUB_SHA"
  git submodule sync --recursive
  git submodule update --init --recursive --depth=1
else
  echo "No cache found, cloning fresh"
  git clone --recurse-submodules --shallow-submodules "$REPO_URL" /repo-cache
  cd /repo-cache
  git fetch --depth=1 origin "$GITHUB_SHA"
  git checkout -f "$GITHUB_SHA"
fi

mkdir -p "$GITHUB_WORKSPACE"
cp -a /repo-cache/. "$GITHUB_WORKSPACE"/

echo "::endgroup::"
