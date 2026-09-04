from __future__ import annotations

import json
import os
import shutil
import subprocess
import tempfile
import unittest
from pathlib import Path
from unittest.mock import patch

from cpp_ci import submodules


class SubmoduleCacheTests(unittest.TestCase):
    def _git(self, repository: Path, *arguments: str) -> None:
        subprocess.run(["git", *arguments], cwd=repository, check=True, stdout=subprocess.DEVNULL)

    def _configure_author(self, repository: Path) -> None:
        self._git(repository, "config", "user.name", "CI test")
        self._git(repository, "config", "user.email", "ci-test@example.invalid")
        self._git(repository, "config", "commit.gpgsign", "false")

    def test_cache_restores_pinned_submodules_without_the_original_remote(self) -> None:
        with tempfile.TemporaryDirectory() as directory, patch.dict(os.environ, {"GIT_ALLOW_PROTOCOL": "file"}):
            root = Path(directory)
            dependency_remote = root / "dependency.git"
            dependency_source = root / "dependency-source"
            superproject = root / "superproject"
            consumer = root / "consumer"
            cache_root = root / "cache"

            subprocess.run(["git", "init", "--bare", str(dependency_remote)], check=True, stdout=subprocess.DEVNULL)
            subprocess.run(["git", "init", str(dependency_source)], check=True, stdout=subprocess.DEVNULL)
            self._configure_author(dependency_source)
            (dependency_source / "dependency.txt").write_text("cached dependency\n", encoding="utf-8")
            self._git(dependency_source, "add", "dependency.txt")
            self._git(dependency_source, "commit", "-m", "initial dependency")
            self._git(dependency_source, "remote", "add", "origin", str(dependency_remote))
            self._git(dependency_source, "push", "origin", "HEAD")

            subprocess.run(["git", "init", str(superproject)], check=True, stdout=subprocess.DEVNULL)
            self._configure_author(superproject)
            (superproject / "README.md").write_text("superproject\n", encoding="utf-8")
            self._git(superproject, "add", "README.md")
            self._git(superproject, "commit", "-m", "initial superproject")
            self._git(
                superproject,
                "-c",
                "protocol.file.allow=always",
                "submodule",
                "add",
                str(dependency_remote),
                "dependencies/dependency",
            )
            self._git(superproject, "commit", "-am", "add dependency")
            self._git(superproject, "submodule", "deinit", "--force", "--all")
            shutil.rmtree(superproject / ".git" / "modules")

            self.assertFalse(
                submodules.prepare(
                    superproject,
                    cache_root=cache_root,
                    cache_namespace="submodule-cache-test",
                )
            )

            subprocess.run(
                ["git", "clone", "--no-recurse-submodules", str(superproject), str(consumer)],
                check=True,
                stdout=subprocess.DEVNULL,
            )
            offline_remote = root / "dependency-offline.git"
            dependency_remote.rename(offline_remote)

            self.assertTrue(
                submodules.prepare(
                    consumer,
                    cache_root=cache_root,
                    cache_namespace="submodule-cache-test",
                )
            )
            cache_paths = submodules.cache_paths(cache_root, "submodule-cache-test")
            consumer_objects = consumer / ".git" / "modules" / "dependencies" / "dependency" / "objects"
            self.assertEqual(
                (consumer_objects / "info" / "alternates").read_text(encoding="utf-8"),
                f"{(cache_paths.objects / 'dependencies' / 'dependency' / 'objects').resolve()}\n",
            )
            self.assertFalse(list((consumer_objects / "pack").glob("*.pack")))
            status = subprocess.run(
                ["git", "submodule", "status", "--recursive"],
                cwd=consumer,
                check=True,
                capture_output=True,
                text=True,
            ).stdout
            self.assertNotIn("-", status)
            self.assertEqual(
                (consumer / "dependencies" / "dependency" / "dependency.txt").read_text(encoding="utf-8"),
                "cached dependency\n",
            )

            offline_remote.rename(dependency_remote)
            (dependency_source / "dependency.txt").write_text("updated dependency\n", encoding="utf-8")
            self._git(dependency_source, "commit", "-am", "update dependency")
            self._git(dependency_source, "push", "origin", "HEAD")
            updated_revision = subprocess.run(
                ["git", "rev-parse", "HEAD"],
                cwd=dependency_source,
                check=True,
                capture_output=True,
                text=True,
            ).stdout.strip()
            self._git(superproject / "dependencies" / "dependency", "fetch", "origin")
            self._git(superproject / "dependencies" / "dependency", "checkout", updated_revision)
            self._git(superproject, "add", "dependencies/dependency")
            self._git(superproject, "commit", "-m", "update dependency pin")

            refreshed_consumer = root / "refreshed-consumer"
            subprocess.run(
                ["git", "clone", "--no-recurse-submodules", str(superproject), str(refreshed_consumer)],
                check=True,
                stdout=subprocess.DEVNULL,
            )
            self.assertFalse(
                submodules.prepare(
                    refreshed_consumer,
                    cache_root=cache_root,
                    cache_namespace="submodule-cache-test",
                )
            )
            self.assertEqual(
                (refreshed_consumer / "dependencies" / "dependency" / "dependency.txt").read_text(
                    encoding="utf-8"
                ),
                "updated dependency\n",
            )

            dependency_remote.rename(offline_remote)
            updated_consumer = root / "updated-consumer"
            subprocess.run(
                ["git", "clone", "--no-recurse-submodules", str(superproject), str(updated_consumer)],
                check=True,
                stdout=subprocess.DEVNULL,
            )
            self.assertTrue(
                submodules.prepare(
                    updated_consumer,
                    cache_root=cache_root,
                    cache_namespace="submodule-cache-test",
                )
            )
            self.assertEqual(
                (updated_consumer / "dependencies" / "dependency" / "dependency.txt").read_text(
                    encoding="utf-8"
                ),
                "updated dependency\n",
            )

    def test_signature_changes_when_a_direct_gitlink_changes(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            repository = Path(directory)
            subprocess.run(["git", "init", str(repository)], check=True, stdout=subprocess.DEVNULL)
            self._configure_author(repository)
            (repository / ".gitmodules").write_text(
                "[submodule \"dependency\"]\n\tpath = dependency\n\turl = https://example.invalid/dependency.git\n",
                encoding="utf-8",
            )
            self._git(repository, "add", ".gitmodules")
            self._git(repository, "commit", "-m", "add module definition")

            index = repository / ".git" / "index"
            subprocess.run(
                [
                    "git",
                    "update-index",
                    "--add",
                    "--cacheinfo",
                    "160000",
                    "1111111111111111111111111111111111111111",
                    "dependency",
                ],
                cwd=repository,
                check=True,
            )
            first = submodules.submodule_signature(repository)
            subprocess.run(
                [
                    "git",
                    "update-index",
                    "--cacheinfo",
                    "160000",
                    "2222222222222222222222222222222222222222",
                    "dependency",
                ],
                cwd=repository,
                check=True,
            )
            second = submodules.submodule_signature(repository)

            self.assertIsNotNone(first)
            self.assertIsNotNone(second)
            self.assertNotEqual(first, second)
            self.assertTrue(index.is_file())

    def test_cache_restores_nested_pinned_submodules_without_remotes(self) -> None:
        with tempfile.TemporaryDirectory() as directory, patch.dict(os.environ, {"GIT_ALLOW_PROTOCOL": "file"}):
            root = Path(directory)
            leaf_remote = root / "leaf.git"
            leaf_source = root / "leaf-source"
            parent_remote = root / "parent.git"
            parent_source = root / "parent-source"
            superproject = root / "superproject"
            consumer = root / "consumer"
            cache_root = root / "cache"

            subprocess.run(["git", "init", "--bare", str(leaf_remote)], check=True, stdout=subprocess.DEVNULL)
            subprocess.run(["git", "init", str(leaf_source)], check=True, stdout=subprocess.DEVNULL)
            self._configure_author(leaf_source)
            (leaf_source / "leaf.txt").write_text("nested cached dependency\n", encoding="utf-8")
            self._git(leaf_source, "add", "leaf.txt")
            self._git(leaf_source, "commit", "-m", "initial leaf")
            self._git(leaf_source, "remote", "add", "origin", str(leaf_remote))
            self._git(leaf_source, "push", "origin", "HEAD")

            subprocess.run(["git", "init", "--bare", str(parent_remote)], check=True, stdout=subprocess.DEVNULL)
            subprocess.run(["git", "init", str(parent_source)], check=True, stdout=subprocess.DEVNULL)
            self._configure_author(parent_source)
            (parent_source / "parent.txt").write_text("parent dependency\n", encoding="utf-8")
            self._git(parent_source, "add", "parent.txt")
            self._git(parent_source, "commit", "-m", "initial parent")
            self._git(
                parent_source,
                "-c",
                "protocol.file.allow=always",
                "submodule",
                "add",
                str(leaf_remote),
                "dependencies/leaf",
            )
            self._git(parent_source, "commit", "-am", "add nested leaf")
            self._git(parent_source, "remote", "add", "origin", str(parent_remote))
            self._git(parent_source, "push", "origin", "HEAD")

            subprocess.run(["git", "init", str(superproject)], check=True, stdout=subprocess.DEVNULL)
            self._configure_author(superproject)
            (superproject / "README.md").write_text("superproject\n", encoding="utf-8")
            self._git(superproject, "add", "README.md")
            self._git(superproject, "commit", "-m", "initial superproject")
            self._git(
                superproject,
                "-c",
                "protocol.file.allow=always",
                "submodule",
                "add",
                str(parent_remote),
                "dependencies/parent",
            )
            self._git(superproject, "commit", "-am", "add parent")
            self._git(superproject, "submodule", "deinit", "--force", "--all")
            shutil.rmtree(superproject / ".git" / "modules")

            self.assertFalse(
                submodules.prepare(
                    superproject,
                    cache_root=cache_root,
                    cache_namespace="nested-submodule-cache-test",
                )
            )
            subprocess.run(
                ["git", "clone", "--no-recurse-submodules", str(superproject), str(consumer)],
                check=True,
                stdout=subprocess.DEVNULL,
            )
            parent_remote.rename(root / "parent-offline.git")
            leaf_remote.rename(root / "leaf-offline.git")

            self.assertTrue(
                submodules.prepare(
                    consumer,
                    cache_root=cache_root,
                    cache_namespace="nested-submodule-cache-test",
                )
            )
            self.assertEqual(
                (consumer / "dependencies" / "parent" / "dependencies" / "leaf" / "leaf.txt").read_text(
                    encoding="utf-8"
                ),
                "nested cached dependency\n",
            )
            cache_paths = submodules.cache_paths(cache_root, "nested-submodule-cache-test")
            nested_alternates = (
                consumer
                / ".git"
                / "modules"
                / "dependencies"
                / "parent"
                / "modules"
                / "dependencies"
                / "leaf"
                / "objects"
                / "info"
                / "alternates"
            )
            self.assertEqual(
                nested_alternates.read_text(encoding="utf-8"),
                f"{(cache_paths.objects / 'dependencies' / 'parent' / 'modules' / 'dependencies' / 'leaf' / 'objects').resolve()}\n",
            )

    def test_legacy_metadata_cache_migrates_without_downloading_packs(self) -> None:
        with tempfile.TemporaryDirectory() as directory, patch.dict(os.environ, {"GIT_ALLOW_PROTOCOL": "file"}):
            root = Path(directory)
            dependency_remote = root / "dependency.git"
            dependency_source = root / "dependency-source"
            superproject = root / "superproject"
            consumer = root / "consumer"
            cache_root = root / "cache"
            namespace = "legacy-submodule-cache-test"

            subprocess.run(["git", "init", "--bare", str(dependency_remote)], check=True, stdout=subprocess.DEVNULL)
            subprocess.run(["git", "init", str(dependency_source)], check=True, stdout=subprocess.DEVNULL)
            self._configure_author(dependency_source)
            (dependency_source / "dependency.txt").write_text("legacy cached dependency\n", encoding="utf-8")
            self._git(dependency_source, "add", "dependency.txt")
            self._git(dependency_source, "commit", "-m", "initial dependency")
            self._git(dependency_source, "remote", "add", "origin", str(dependency_remote))
            self._git(dependency_source, "push", "origin", "HEAD")

            subprocess.run(["git", "init", str(superproject)], check=True, stdout=subprocess.DEVNULL)
            self._configure_author(superproject)
            (superproject / "README.md").write_text("superproject\n", encoding="utf-8")
            self._git(superproject, "add", "README.md")
            self._git(superproject, "commit", "-m", "initial superproject")
            self._git(
                superproject,
                "-c",
                "protocol.file.allow=always",
                "submodule",
                "add",
                str(dependency_remote),
                "dependencies/dependency",
            )
            self._git(superproject, "commit", "-am", "add dependency")

            legacy_paths = submodules.cache_paths(cache_root, namespace)
            legacy_paths.root.mkdir(parents=True)
            shutil.copytree(superproject / ".git" / "modules", legacy_paths.legacy_modules)
            signature = submodules.submodule_signature(superproject)
            self.assertIsNotNone(signature)
            legacy_paths.metadata.write_text(
                '{"format": 1, "signature": "' + signature + '"}\n',
                encoding="utf-8",
            )

            subprocess.run(
                ["git", "clone", "--no-recurse-submodules", str(superproject), str(consumer)],
                check=True,
                stdout=subprocess.DEVNULL,
            )
            dependency_remote.rename(root / "dependency-offline.git")

            self.assertTrue(
                submodules.prepare(
                    consumer,
                    cache_root=cache_root,
                    cache_namespace=namespace,
                )
            )
            self.assertEqual(
                (consumer / "dependencies" / "dependency" / "dependency.txt").read_text(encoding="utf-8"),
                "legacy cached dependency\n",
            )
            metadata = json.loads(legacy_paths.metadata.read_text(encoding="utf-8"))
            self.assertEqual(metadata["format"], submodules.CACHE_FORMAT)
            self.assertTrue(legacy_paths.metadata_modules.is_dir())


if __name__ == "__main__":
    unittest.main()
