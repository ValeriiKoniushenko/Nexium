---
name: clean-build
description: Full clean rebuild from scratch, bypassing all caches. Use when the user reports stale/weird build state, after switching branches with structural CMake changes, or when incremental build fails inexplicably.
---

# Clean Build

Nukes the build dir and cache; use only when incremental build is untrustworthy.

## Command

```bash
rm -rf build
ccache -C          # clear ccache too — sometimes the corruption is in the cache, not the build dir
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug
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