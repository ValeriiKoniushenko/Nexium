---
name: lint-and-format
description: Run clang-format and clang-tidy locally before committing/pushing. Use before opening a PR, after finishing a change, or if the user asks to check code style or lint issues.
---

# Lint & Format

Mirrors the checks that run on gitea.vakon.dev CI (clang-format, clang-tidy jobs)
— catch failures locally instead of waiting for the pipeline.

## Format check

```
clang-format --dry-run --Werror $(git diff --name-only --diff-filter=ACMR -- '*.cpp' '*.h' '*.hpp')
```

## Format fix

```
clang-format -i $(git diff --name-only --diff-filter=ACMR -- '*.cpp' '*.h' '*.hpp')
```

## Tidy check

Auto-detect the active build dir by finding the most recently modified compile_commands.json across known build layouts:

```
BUILD_DIR=$(find build -mindepth 1 -maxdepth 3 -name compile_commands.json \
-printf '%T@ %h\n' 2>/dev/null | sort -rn | head -1 | cut -d' ' -f2-)

if [ -z "$BUILD_DIR" ]; then echo "No compile_commands.json found under build/ — run cmake configure first."
exit 1 fi

run-clang-tidy -p "$BUILD_DIR" -j 16 "$(realpath ./sources)/.*\. (cpp|cxx|cc)$"
```

## Valgrind check

```
valgrind --leak-check=full --show-leak-kinds=definite --track-origins=yes build/bin/Nexium_Tests
```

## Known build layouts

- build/
- build/clang/debug
- build/clang/release
- build/gcc/debug
- build/gcc/release

If more than one exists, the most recently configured/built one wins (mtime of compile_commands.json). Force a specific
one by setting BUILD_DIR yourself before running this skill.

## Notes

- Only lints changed files, not the whole tree — full-tree clang-tidy is slow and CI already covers it.
- Run this before `git push`, not before every local build.