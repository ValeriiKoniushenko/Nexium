#!/usr/bin/env bash
# Restores previously built binaries from the shared /build-cache mount.
set -euo pipefail

echo "::group::Restore build output"
mkdir -p build
rsync -a /build-cache/ build/
echo "::endgroup::"
