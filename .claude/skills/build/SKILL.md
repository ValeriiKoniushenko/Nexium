---
name: build
description: Standard incremental build of the project. Use when the user asks to build, compile, or check that changes compile.
---

# Build

Standard build — reuses ccache/build cache, only rebuilds changed files.

## Command

### Linux / macOS

```bash
cmake --build build -j$(nproc)
```

### Windows

```powershell
cmake --build build --parallel
```

## Prerequisites

- Build dir must exist (`cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug`) — run once, not part of this skill.
- ccache is expected to be on PATH and picked up automatically via CMAKE_CXX_COMPILER_LAUNCHER.
- Try to check if the build dir is already existing by the next path: `build/{clang,gcc}/{debug,release}`

## Resulting binaries

- `<build_dir>/bin/TemplateGame` - template game executable with real editor UI.
- `<build_dir>/bin/Nexium_Tests` - Nexium (game engine) tests.

## Notes

Don't suggest `--clean-first` here — that's the separate clean-build skill.