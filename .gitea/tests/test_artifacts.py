from __future__ import annotations

import json
import tempfile
import unittest
from pathlib import Path

from cpp_ci import artifacts
from cpp_ci.config import load_project_configuration


class ArtifactTests(unittest.TestCase):
    def test_stage_and_restore_preserves_the_profile_runtime_file(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            binary = root / "build" / "ci" / "clang-debug" / "bin" / "Tests"
            binary.parent.mkdir(parents=True)
            binary.write_bytes(b"test executable")
            binary.chmod(0o755)
            config_path = root / "ci.json"
            config_path.write_text(
                json.dumps(
                    {
                        "schema_version": 1,
                        "project_id": "artifact-test",
                        "base_ref": "develop",
                        "unit_tests": {"command": ["{build_dir}/bin/Tests"]},
                        "artifacts": {"paths": ["{build_dir}/bin/Tests"]},
                    }
                ),
                encoding="utf-8",
            )
            config = load_project_configuration(config_path, project_root=root)
            profile = config.profile("clang-debug")
            archive = artifacts.stage(config, profile)
            binary.unlink()
            artifacts.restore(config, profile, archive=archive)
            self.assertEqual(binary.read_bytes(), b"test executable")
            self.assertTrue(binary.stat().st_mode & 0o111)


if __name__ == "__main__":
    unittest.main()
