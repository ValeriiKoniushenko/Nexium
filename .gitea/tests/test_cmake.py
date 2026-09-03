from __future__ import annotations

import json
import tempfile
import unittest
from pathlib import Path
from unittest.mock import patch

from cpp_ci import cmake
from cpp_ci.config import load_project_configuration


class CmakeCommandTests(unittest.TestCase):
    def test_configure_mandates_compilers_and_both_ccache_launchers(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            (root / "CMakeLists.txt").write_text("cmake_minimum_required(VERSION 3.30)\n", encoding="utf-8")
            config_path = root / "ci.json"
            config_path.write_text(
                json.dumps(
                    {
                        "schema_version": 1,
                        "project_id": "cmake-test",
                        "base_ref": "develop",
                        "tools": {
                            "ccache": "ccache",
                            "toolchains": {"clang": {"c": "clang-22", "cxx": "clang++-22"}},
                        },
                        "unit_tests": {"command": ["{build_dir}/bin/Tests"]},
                        "artifacts": {"paths": ["{build_dir}/bin/Tests"]},
                    }
                ),
                encoding="utf-8",
            )
            config = load_project_configuration(config_path, project_root=root)
            calls: list[list[str]] = []

            def record(command, **_kwargs):
                calls.append(command)

            with patch("cpp_ci.cmake.show_toolchain"), patch("cpp_ci.cmake.subprocess.run", side_effect=record):
                cmake.configure(config, config.profile("clang-debug"))

            command = calls[-1]
            self.assertIn("-DCMAKE_C_COMPILER=clang-22", command)
            self.assertIn("-DCMAKE_CXX_COMPILER=clang++-22", command)
            self.assertIn("-DCMAKE_C_COMPILER_LAUNCHER=ccache", command)
            self.assertIn("-DCMAKE_CXX_COMPILER_LAUNCHER=ccache", command)
            self.assertIn("-DCMAKE_EXPORT_COMPILE_COMMANDS=ON", command)
            self.assertIn("-DCMAKE_BUILD_TYPE=Debug", command)


if __name__ == "__main__":
    unittest.main()
