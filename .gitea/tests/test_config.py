from __future__ import annotations

import json
import tempfile
import unittest
from pathlib import Path

from cpp_ci.config import ConfigurationError, load_project_configuration


ROOT = Path(__file__).resolve().parents[2]


class ProjectConfigurationTests(unittest.TestCase):
    def test_nexium_profile_has_a_separate_build_directory(self) -> None:
        config = load_project_configuration(ROOT / ".ci" / "cpp-ci.json", project_root=ROOT)

        clang_debug = config.profile("clang-debug")
        gcc_release = config.profile("gcc-release")

        self.assertEqual(clang_debug.build_type, "Debug")
        self.assertEqual(gcc_release.build_type, "Release")
        self.assertNotEqual(clang_debug.build_dir, gcc_release.build_dir)
        self.assertTrue(str(clang_debug.build_dir).endswith("build/ci/clang-debug"))
        self.assertEqual(config.tools["clang_format"], "clang-format-22")

    def test_project_id_is_validated_before_it_can_name_host_cache_paths(self) -> None:
        invalid = {
            "schema_version": 1,
            "project_id": "../../unsafe",
            "base_ref": "develop",
            "unit_tests": {"command": ["build/bin/tests"]},
            "artifacts": {"paths": ["build/bin/tests"]},
        }
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "ci.json"
            path.write_text(json.dumps(invalid), encoding="utf-8")
            with self.assertRaises(ConfigurationError):
                load_project_configuration(path, project_root=ROOT)


if __name__ == "__main__":
    unittest.main()
