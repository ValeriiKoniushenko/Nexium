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

    def test_compilation_database_artifact_relocates_the_producer_checkout(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            producer = root / "producer"
            consumer = root / "consumer"
            config_data = {
                "schema_version": 1,
                "project_id": "artifact-test",
                "base_ref": "develop",
                "unit_tests": {"command": ["{build_dir}/bin/Tests"]},
                "artifacts": {"paths": ["{build_dir}/bin/Tests"]},
            }
            producer.mkdir()
            producer_config_path = producer / "ci.json"
            producer_config_path.write_text(json.dumps(config_data), encoding="utf-8")
            producer_config = load_project_configuration(producer_config_path, project_root=producer)
            profile = producer_config.profile("clang-debug")
            compilation_database = profile.build_dir / "compile_commands.json"
            compilation_database.parent.mkdir(parents=True)
            compilation_database.write_text(
                json.dumps(
                    [
                        {
                            "directory": str(producer),
                            "file": str(producer / "sources" / "Example.cpp"),
                            "command": f"clang++ -I{producer}/sources -c {producer}/sources/Example.cpp",
                        }
                    ]
                ),
                encoding="utf-8",
            )
            generated_header = producer / "sources" / "Example.generated.h"
            generated_header.parent.mkdir()
            generated_header.write_text("// generated in producer build\n", encoding="utf-8")
            archive = artifacts.stage_compilation_database(producer_config, profile)

            consumer.mkdir()
            consumer_config_path = consumer / "ci.json"
            consumer_config_path.write_text(json.dumps(config_data), encoding="utf-8")
            consumer_config = load_project_configuration(consumer_config_path, project_root=consumer)
            artifacts.restore_compilation_database(
                consumer_config,
                consumer_config.profile("clang-debug"),
                archive=archive,
            )
            restored = json.loads(
                (consumer / "build" / "ci" / "clang-debug" / "compile_commands.json").read_text(encoding="utf-8")
            )[0]
            self.assertEqual(restored["directory"], str(consumer))
            self.assertEqual(restored["file"], str(consumer / "sources" / "Example.cpp"))
            self.assertNotIn(str(producer), restored["command"])
            self.assertIn(str(consumer), restored["command"])
            self.assertEqual(
                (consumer / "sources" / "Example.generated.h").read_text(encoding="utf-8"),
                "// generated in producer build\n",
            )


if __name__ == "__main__":
    unittest.main()
