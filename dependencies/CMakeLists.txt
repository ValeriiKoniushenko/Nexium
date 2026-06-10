# ── Vendored (already in-tree, no submodule) ──────────────────────────────────
add_subdirectory(Glad)
add_subdirectory(ImGui)
add_subdirectory(Rectpack2D)
add_subdirectory(Stb)

# ── nlohmann/json ─────────────────────────────────────────────────────────────
set(JSON_BuildTests       OFF CACHE BOOL "" FORCE)
set(JSON_MultipleHeaders  OFF CACHE BOOL "" FORCE)
add_subdirectory(nlohmann_json)

# ── GLFW ──────────────────────────────────────────────────────────────────────
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL        OFF CACHE BOOL "" FORCE)
add_subdirectory(glfw)

# ── spdlog ────────────────────────────────────────────────────────────────────
set(SPDLOG_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(SPDLOG_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
set(SPDLOG_BUILD_BENCH    OFF CACHE BOOL "" FORCE)
add_subdirectory(spdlog)

# ── Assimp ────────────────────────────────────────────────────────────────────
set(ASSIMP_BUILD_ZLIB         ON  CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_TESTS        OFF CACHE BOOL "" FORCE)
set(ASSIMP_WARNINGS_AS_ERRORS OFF CACHE BOOL "" FORCE)
add_subdirectory(assimp)

# ── freetype ──────────────────────────────────────────────────────────────────
set(SKIP_INSTALL_ALL  ON  CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(FT_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
set(FT_BUILD_PROGRAMS OFF CACHE BOOL "" FORCE)
set(FT_BUILD_UTILS    OFF CACHE BOOL "" FORCE)
add_subdirectory(freetype)

# ── Utils ─────────────────────────────────────────────────────────────────────
add_subdirectory(Utils)

# ── JustReflectMe ─────────────────────────────────────────────────────────────
set(JRM_DISABLE_TESTS ON)
add_subdirectory(JustReflectMe)