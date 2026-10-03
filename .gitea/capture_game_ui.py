#!/usr/bin/env python3
"""Compatibility launcher for the shared CI helper."""

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))

from scripts.ci.gitea.capture_game_ui import main


if __name__ == "__main__":
    main()
