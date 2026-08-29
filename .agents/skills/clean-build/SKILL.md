---
name: clean-build
description: Full clean rebuild from scratch, bypassing all caches. Use when the user reports stale/weird build state, after switching branches with structural CMake changes, or when incremental build fails inexplicably.
---

# Clean Build

Nukes the build dir and cache; use only when the incremental build is untrustworthy.

## Command

```bash
rm -rf build
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug
```

### Linux / macOS

```bash
cmake --build build -j$(nproc)
```

### Windows

```powershell
cmake --build build --parallel
```

## When to actually reach for this

- CMakeLists.txt changed structurally (new targets, changed compiler/linker flags)
- Switching between MSVC/GCC/Clang toolchains
- ccache double-invocation bug resurfaces
- "it builds in CI but not locally" type reports

## Cost warning

This is slow (no cache reuse). Don't run it reflexively for every failure — try `build` first and only escalate here if
that doesn't explain the issue.