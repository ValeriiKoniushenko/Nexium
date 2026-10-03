#!/usr/bin/env python3
"""Compatibility launcher for the shared CI helper."""

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))

from scripts.ci.gitea.publish_copyright import main


if __name__ == "__main__":
    raise SystemExit(main())
