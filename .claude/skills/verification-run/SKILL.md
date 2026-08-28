---
name: verification-run
description: Verify a change by running the unit tests and the game executable (engine + editor UI) after building. Use this after implementing a change to confirm it actually works, not just that it compiles.
---

# Verification Run

Prerequisite: the project must already be built. Use the `build` skill first — this skill assumes a successful build and
does not build anything itself. `build` below is the build output directory produced by that skill.

## 1. Run unit tests

```
build/bin/Nexium_Tests
```

- **Pass**: process exits with code `0` and no test failures are printed.
- **Fail**: non-zero exit code, or any `FAILED`/`[  FAILED  ]`-style output — treat the change as not verified and
  investigate before proceeding.

## 2. Run the game executable

```
build/bin/TemplateGame --timeout 5
```

- Runs the engine + editor UI and auto-shuts down after 5 seconds.
- **Pass**: process exits cleanly (exit code `0`), no crash, no hang past the timeout, no error/assert output on
  stdout/stderr.
- **Fail**: crash, non-zero exit, hang beyond timeout, or error/exception output — treat as not verified.

## When to consider a change verified

Both steps must pass. A build succeeding alone is **not** sufficient — always run both commands above before calling a
task done.