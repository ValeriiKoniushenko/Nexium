"""Helpers for writing CI reports in fresh runner workspaces."""

from __future__ import annotations

import json
from pathlib import Path
from typing import Any


def write_json_report(report_path: str, payload: Any) -> None:
    """Write a JSON report, creating its parent directory when necessary."""
    path = Path(report_path)
    path.parent.mkdir(parents=True, exist_ok=True)
    with path.open("w", encoding="utf-8") as report:
        json.dump(payload, report, indent=2)
        report.write("\n")
