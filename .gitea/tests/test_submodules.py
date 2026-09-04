from __future__ import annotations

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


if __name__ == "__main__":
    unittest.main()
