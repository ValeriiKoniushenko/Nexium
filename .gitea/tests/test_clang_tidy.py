from __future__ import annotations

import os
import subprocess
import tempfile
import unittest
from pathlib import Path
from unittest.mock import patch

from cpp_ci.clang_tidy import run_clang_tidy


class ClangTidyTests(unittest.TestCase):
    def test_command_uses_the_given_build_directory_as_its_database(self) -> None:
        with patch(
            "cpp_ci.clang_tidy.subprocess.run",
            return_value=subprocess.CompletedProcess([], 0, "", ""),
        ) as run:
            run_clang_tidy("sources/Thing.cpp", "build/ci/clang-tidy", "", [])

        command = run.call_args.args[0]
        database_index = command.index("-p") + 1
        self.assertEqual(
            command[database_index],
            os.path.abspath("build/ci/clang-tidy"),
        )

    def test_missing_compilation_database_is_rejected_before_analysis(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            build_dir = Path(directory) / "build"
            build_dir.mkdir()
            with patch(
                "sys.argv",
                ["clang_tidy.py", "--build-dir", str(build_dir), "--no-gitea"],
            ):
                from cpp_ci import clang_tidy

                with self.assertRaises(SystemExit) as error:
                    clang_tidy.main()

        self.assertEqual(error.exception.code, 2)


if __name__ == "__main__":
    unittest.main()
