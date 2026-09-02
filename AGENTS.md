# AGENTS.md

## Project

Nexium — a personal C++ game engine. Currently focused on 2D/3D scene rendering, an ECS architecture, and an ImGui-based
editor.

## Architecture

- **Rendering**: OpenGL. GLSL infinite grid shaders (LOD, AA, axis highlighting) for both orthographic and perspective
  cameras.
- **ECS**: components self-register via linker sections (no manual registry list — new components just need the
  macro/section attribute).
- **Editor**: ImGui, docking layout, tree views, property panels. Object picking via ray casting, works for both camera
  projections.
- **Assets**: texture atlases, image loading via stb.

## Build

- CMake, cross-platform (Linux, Windows): MSVC / GCC / Clang.
- MSVC debug info: use `/Z7`, not `/Zi` — required for ccache compatibility.
- CI runs on self-hosted Gitea (gitea.vakon.dev) via act_runner:
  clang-format, clang-tidy, build (GCC+Clang), unit tests, valgrind.

## Project Workflows

Shared skills under `.agents/skills/` are generic. Read this section for the concrete
Nexium commands, target names, paths, and options.

- The standard configured build directory is `build/`; legacy local layouts may also use
  `build/{clang,gcc}/{debug,release}`. Configure the standard debug build with:

  ```sh
  cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug
  ```

- Build incrementally with `cmake --build build --parallel` (or `-j$(nproc)` on Linux).
  A clean build removes only `build/`, then reruns the standard configuration and build.
- The test target and executable are `Nexium_Tests` and `build/bin/Nexium_Tests`.
  The editor smoke-test executable is `build/bin/TemplateGame --timeout 5`; verification
  requires both it and the unit-test executable to exit successfully.
- CMake runs `jrm ${CMAKE_SOURCE_DIR}` through the `CodeGenerator` target. After changing
  a JRM-reflected or annotated type, or when generated metadata is stale, delete
  `.jrm/cache.data` (`Remove-Item .jrm/cache.data` in PowerShell) and run a normal build.
- ECS performance work uses the `Nexium_ECS_Benchmarks` target from
  `benchmarks/ECS/ECSBenchmarks.cpp`, producing `build/bin/Nexium_ECS_Benchmarks`.
  Build only that target with
  `cmake --build build --parallel --target Nexium_ECS_Benchmarks`. List cases with
  `build/bin/Nexium_ECS_Benchmarks --benchmark_list_tests`; use the benchmark filter,
  repetitions, and aggregate-report options for focused comparison runs. Benchmark-only
  components are not JRM-reflected; run code generation first if an annotated engine
  header changes.
- Format changed C++ files with `clang-format`; CI runs clang-format, clang-tidy, the GCC
  and Clang builds, unit tests, and valgrind. Select the active `compile_commands.json`
  from the configured build directory before running `run-clang-tidy`. The project memory
  check is:

  ```sh
  valgrind --leak-check=full --show-leak-kinds=definite --track-origins=yes \
    build/bin/Nexium_Tests
  ```

## Conventions

- Target standard: **C++26** (`-std=c++2c` on GCC/Clang). Support is still partial across compilers — verify a given
  feature is actually available on the toolchain/compiler version in use before relying on it, rather than assuming full
  C++26 conformance.
- MSVC in particular lags GCC/Clang on C++26 feature coverage — check before using newer standard features in code that
  must build on MSVC.
- Do **not** confuse native C++26 reflection (`std::meta`, P2996) with this project's own **JustReflectMe (JRM)**
  system — they are unrelated. JRM is the reflection mechanism actually used in this codebase (see the JRM section
  above); native reflection is not currently in use here.
- Prefer modern standard-library facilities over hand-rolled equivalents when available on the target compilers, but
  check availability first — don't assume a C++23/26 feature compiles everywhere this project builds.
- **Reflection (JustReflectMe / JRM)**: mark types with `CLASS()` / `ENUM_CLASS()`
  macros before the relevant keyword. Full details live in
  `sources/JustReflectMe/Adapter.h` — read that before generating or modifying reflected types.
  - Generated files update automatically as part of the normal build.
  - To force a full regen, delete `.jrm/cache.data`.
- **Strings**: use `Core::StringAtom` + the `_atom` literal for interned/ static strings — don't use raw `std::string`/
  `const char*` for these.
- **Smart pointers**: use `Core::IntrusivePtr` / `Core::WeakPtr`, not
  `std::shared_ptr`/`std::weak_ptr`.
- **`dependencies/` folder**: these are separate real repos, not engine code. Generally out of scope for engine-side
  changes; exceptions are
  `Utils` and `JustReflectMe`, where cross-cutting fixes may be relevant.
- **Testing**: TDD-leaning — add at least minimal tests alongside new implementation, not after. Tests live in `tests/`.
- Prefer `std::array` over C arrays (mid-migration — match existing style in the file you're editing).
- clang-tidy must pass; don't introduce new suppressions without reason.
- New ECS components: use the self-registration macro, don't hand-wire into a central list.

## Verification

- After implementing a change, verify it before considering it done:
  1. **Build** — use the `build` skill to compile and confirm there are no build errors/warnings introduced.
  2. **Run** — use the `verification-run` skill to run the result and confirm it behaves as expected.
- Don't treat a change as complete on "looks correct" alone — actually build and run it via these skills first.
