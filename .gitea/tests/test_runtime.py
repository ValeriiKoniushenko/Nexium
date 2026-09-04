from __future__ import annotations

import argparse
import subprocess
import unittest
from pathlib import Path
from unittest.mock import patch

from cpp_ci import runtime


class RuntimeTests(unittest.TestCase):
    def test_separator_is_not_part_of_the_application_command(self) -> None:
        self.assertEqual(
            runtime._application_command(["--", "build/bin/TemplateGame", "--timeout", "5"]),
            ["build/bin/TemplateGame", "--timeout", "5"],
        )

    def test_runtime_valgrind_does_not_forward_the_separator(self) -> None:
        args = argparse.Namespace(
            command=["--", "build/bin/TemplateGame"],
            valgrind="valgrind",
            timeout=5.0,
            expect_timeout=False,
            headless=False,
            display=":99",
            screen_size=(1920, 1080),
            suppression=[],
            generate_suppressions=False,
            environment=[],
        )
        completed = subprocess.CompletedProcess([], 0, "", "ERROR SUMMARY: 0 errors from 0 contexts\n")
        with (
            patch("cpp_ci.runtime.shutil.which", return_value="/usr/bin/tool"),
            patch("cpp_ci.runtime.subprocess.run", return_value=completed) as run,
        ):
            self.assertEqual(runtime.run_under_valgrind(args), 0)

        command = run.call_args.args[0]
        self.assertIn("build/bin/TemplateGame", command)
        self.assertNotIn("--", command)

    def test_root_capture_falls_back_to_imagemagick_when_scrot_is_unavailable(self) -> None:
        output = Path("/tmp/cpp-ci-runtime-test.png")
        completed = subprocess.CompletedProcess([], 0, "", "")

        with (
            patch("cpp_ci.runtime.shutil.which", side_effect=lambda name: "/usr/bin/import" if name == "import" else None),
            patch("cpp_ci.runtime.subprocess.run", return_value=completed) as run,
            patch.object(Path, "is_file", return_value=True),
            patch.object(Path, "stat", return_value=type("Stat", (), {"st_size": 1})()),
        ):
            runtime._capture(output, window_title=None, environment={"DISPLAY": ":99"})

        self.assertEqual(run.call_args.args[0], ["import", "-window", "root", str(output)])


if __name__ == "__main__":
    unittest.main()
