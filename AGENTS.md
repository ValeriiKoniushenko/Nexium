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

- CMake 3.30 or newer, cross-platform (Linux, Windows): MSVC / GCC / Clang.
- MSVC debug info: use `/Z7`, not `/Zi` — required for ccache compatibility.
- CI runs on self-hosted Gitea (gitea.vakon.dev) via act_runner:
  clang-format, clang-tidy, build (GCC+Clang), unit tests, valgrind.

## Project Workflows

The skills linked under `.agents/skills/` are repository-independent. Every linked skill
must use the project-specific configuration below instead of guessing targets, paths,
options, or pass criteria. The sections intentionally match the skill names.

### Shared configuration

- Run every command from the repository root unless a workflow says otherwise.
- The standard build directory is `build/`; legacy local layouts may also use
  `build/{clang,gcc}/{debug,release}`. Substitute the selected configured directory in
  every command and artifact path consistently.
- CMake writes executables to `<build_dir>/bin/` and libraries to `<build_dir>/lib/`.
- Tests and benchmarks are enabled by default. Use `-DNEXIUM_DISABLE_TESTS=ON` or
  `-DNEXIUM_DISABLE_BENCHMARKS=ON` only when the requested workflow does not need them.
- If required submodules are missing, synchronize their recorded configuration and check
  out the revisions pinned by this repository:

  ```sh
  git submodule sync --recursive
  git submodule update --init --recursive
  ```

  Do not add `--remote`, force submodule contents, or update submodules beyond the pinned
  revisions unless the user explicitly requests that operation.

### `build`

Configure the standard Debug/Ninja build when `build/` is absent or unconfigured, then
build it incrementally:

```sh
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel
```

CMake exports `build/compile_commands.json`. The normal all-target build also runs the
`CodeGenerator` target and builds `Nexium_Tests`, `Nexium_ECS_Benchmarks`, and
`TemplateGame` unless their enclosing feature is disabled. Current Linux CI uses Clang
19.1.7 and GCC 14.2.0; Windows uses MSVC-compatible configuration.

### `clean-build`

Use a clean build only for stale/inconsistent output, structural CMake changes, or a
toolchain switch. Remove only the selected build directory, never source, generated JRM
headers, dependencies, tests, benchmarks, documentation, or configuration:

```sh
rm -rf build
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel
```

Initialize missing submodules with the shared pinned-revision commands before configuring.

### `test`

The unit-test target is `Nexium_Tests`, and its executable is
`build/bin/Nexium_Tests`:

```sh
cmake --build build --parallel --target Nexium_Tests
build/bin/Nexium_Tests
```

The project does not currently register tests with CTest, so run the executable directly.
GoogleTest filters such as `--gtest_filter='<suite-or-test-pattern>'` are acceptable for
focused iteration, but run the complete executable before reporting a behavior change as
verified.

### `verification-run`

First complete the `build` workflow. Verification then requires both commands below to
exit successfully:

```sh
build/bin/Nexium_Tests
build/bin/TemplateGame --timeout 5
```

`TemplateGame` needs a working graphical/OpenGL environment. If one is unavailable, report
the smoke check as unavailable rather than silently skipping it. A code change is verified
only after both the unit-test suite and smoke check pass.

### `codegen`

JustReflectMe (JRM), not native C++ reflection, is the generator used by this repository.
CMake defines the always-built `CodeGenerator` target as `jrm ${CMAKE_SOURCE_DIR}`.
Generated `*.generated.h` files live beside their owning headers under `sources/` and
`tests/`; do not edit them manually.

Regenerate after changing a `CLASS()`/`ENUM_CLASS()` type or another JRM annotation, after
changing the JRM adapter/configuration, or when generated metadata is stale. For a forced
regeneration, remove `.jrm/cache.data` (`Remove-Item .jrm/cache.data` in PowerShell), then
run:

```sh
cmake --build build --parallel --target CodeGenerator
cmake --build build --parallel
```

Report generated-file changes separately from hand-written source changes.

### `lint-and-format`

CI uses clang-format 19.1.7 and clang-tidy 19.1.7. Use the project helpers so file
selection and diagnostic handling match CI, and pass `--no-gitea` for local checks:

```sh
python3 .gitea/check_clang_format.py --base develop --no-gitea
python3 .gitea/check_clang_tidy.py --base develop --build-dir build --fail-on error --no-gitea
```

Apply formatting with:

```sh
python3 .gitea/check_clang_format.py --base develop --fix --no-gitea
```

The branch-based helpers compare committed changes with `develop`. For uncommitted task
changes or when that ref is unavailable, pass the exact changed C++ paths with `--files`
instead. Point `--build-dir` at the directory whose `compile_commands.json` matches the
active toolchain. The helper excludes `dependencies/`, `docs/`, `cmake/`, and `data/` from
branch-based C++ analysis; do not widen that scope unless requested.

### `valgrind`

Valgrind checks are Linux-only and require a Debug build, the `valgrind` executable, and
the repository-root `valgrind.supp`. Build and run the CI-aligned unit-test wrapper:

```sh
cmake --build build --parallel --target Nexium_Tests
python3 .gitea/check_valgrind.py --executable build/bin/Nexium_Tests --no-gitea --verbose
```

Pass focused GoogleTest arguments after `--` only while investigating; run the full suite
for final verification. The wrapper treats invalid accesses, uninitialized reads, definite
leaks, test failures, and Valgrind startup failures as failures. CI also checks the game in
a headless Xvfb environment with:

```sh
python3 .gitea/run_game_valgrind.py \
  --executable build/bin/TemplateGame --timeout 5 --no-gitea
```

### `benchmark`

The repository currently has one Google Benchmark executable: the ECS suite described in
the next section. Performance conclusions require a Release build with benchmark targets
enabled:

```sh
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DNEXIUM_DISABLE_BENCHMARKS=OFF
cmake --build build --parallel --target Nexium_ECS_Benchmarks
```

Use the same machine, compiler, build configuration, benchmark filters, and power mode for
before/after comparisons. Store JSON output under the selected build directory, and report
the complete command, configuration, repetitions, benchmark names/arguments, and aggregate
`real_time` values. Do not use Debug results for performance conclusions.

### `ecs-benchmarks`

The ECS benchmark source is `benchmarks/ECS/ECSBenchmarks.cpp`, its CMake target is
`Nexium_ECS_Benchmarks`, and its standard executable is
`build/bin/Nexium_ECS_Benchmarks`. Discover registered cases before selecting a filter:

```sh
build/bin/Nexium_ECS_Benchmarks --benchmark_list_tests
build/bin/Nexium_ECS_Benchmarks --benchmark_filter='<regular-expression>'
```

For a comparison-quality run:

```sh
build/bin/Nexium_ECS_Benchmarks \
  --benchmark_filter='<regular-expression>' \
  --benchmark_repetitions=10 \
  --benchmark_report_aggregates_only=true \
  --benchmark_format=json \
  --benchmark_out=build/ecs-benchmark-results.json
```

Run the complete executable before concluding that an ECS performance change is safe.
Benchmark-only components are deliberately not JRM-reflected; run `codegen` first only
when an annotated engine header used by the benchmark changed.

### `code-coverage`

Nexium currently defines no engine coverage CMake option, instrumentation configuration,
coverage target, report generator, or report output path. Coverage support found inside
dependency repositories is not the engine's coverage workflow and must not be reused as
one. Therefore the `code-coverage` skill must report coverage as unsupported for the root
project rather than claim success or invent commands. Adding root-project coverage support
is a separate implementation task; when it exists, use a dedicated directory such as
`build/coverage` because instrumentation changes compilation and report generation can
rewrite profiling data.

### `docs-generation`

Antora documentation sources live under `docs/nexium/modules/`; navigation is
`docs/nexium/modules/ROOT/nav.adoc`, the component descriptor is
`docs/nexium/antora.yml`, and the playbook is `docs/antora-playbook.yml`. `docs/html/` is
generated output: never use it as a source, read it for documentation facts, or edit it by
hand.

When adding, removing, or renaming a page, update the navigation. Real source/test examples
must be tagged in their owning file and included with an AsciiDoc `include::` directive;
do not paste a duplicate code sample into a documentation page. After every documentation
change, run the required site build from `docs/`:

```sh
npx antora --fetch antora-playbook.yml
```

The build requires Node.js/npm access and may download the Antora CLI or UI bundle. Report
missing tools, network failures, unresolved includes, or site-build failures explicitly.

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
