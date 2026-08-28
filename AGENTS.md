# AGENTS.md

## Project
Nexium — a personal C++ game engine. Currently focused on 2D/3D scene
rendering, an ECS architecture, and an ImGui-based editor.

## Architecture
- **Rendering**: OpenGL. GLSL infinite grid shaders (LOD, AA, axis
  highlighting) for both orthographic and perspective cameras.
- **ECS**: components self-register via linker sections (no manual
  registry list — new components just need the macro/section attribute).
- **Editor**: ImGui, docking layout, tree views, property panels.
  Object picking via ray casting, works for both camera projections.
- **Assets**: texture atlases, image loading via stb.

## Build
- CMake, cross-platform (Linux, Windows): MSVC / GCC / Clang.
- MSVC debug info: use `/Z7`, not `/Zi` — required for ccache compatibility.
- CI runs on self-hosted Gitea (gitea.vakon.dev) via act_runner:
  clang-format, clang-tidy, build (GCC+Clang), unit tests, valgrind.

## Conventions
- Prefer `C++26` over C arrays (mid-migration — match existing style
  in the file you're editing).
- clang-tidy must pass; don't introduce new suppressions without reason.
- New ECS components: use the self-registration macro, don't hand-wire
  into a central list.

## Known gotchas
- ccache + MSVC: double-invocation bug if flags aren't set correctly (see
  CMake config for the workaround).
- MinGW: watch for multiple-definition linker errors with header-only-style
  patterns.