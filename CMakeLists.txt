cmake_minimum_required(VERSION 3.30..)

project(Nexium VERSION 1.0 LANGUAGES CXX)

cmake_policy(SET CMP0141 NEW)
set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT
        "$<$<CONFIG:Debug,RelWithDebInfo>:Embedded>"
)

if (CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
    find_program(CCACHE_PROGRAM ccache)
    if (CCACHE_PROGRAM)
        set(CMAKE_C_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
        set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
        message(STATUS "ccache: ${CCACHE_PROGRAM}")
    endif ()
endif ()

set(CMAKE_CXX_SCAN_FOR_MODULES OFF)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_compile_definitions(NEXIUM_PROJECT_DIR="${CMAKE_CURRENT_SOURCE_DIR}")

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Flags.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Features.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Predefinitions.cmake)


# ── Base setup & Predefinitions ────────────────────────────────────────────────
# CXX standard installing
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(REQUIRED_CXX_STANDARD cxx_std_23)   # Put preferable cxx standard. It will be checked and exported to CXX_STANDARD
set(CXX_STANDARD)                       # Compiler specific value that was going from REQUIRED_CXX_STANDARD
InstallCXXStandardVersion()

# Installing gtest benchmark options
set(BENCHMARK_ENABLE_TESTING)
InstallBenchmarkDependenciesTesting()

# Installing of debug rules
InstallDebugLevel()

add_custom_target(CodeGenerator ALL
    COMMAND jrm ${CMAKE_SOURCE_DIR}
)


# ── Graphics backend selection ────────────────────────────────────────────────
#
# Usage:
#   cmake -B build -DNEXIUM_GRAPHICS_BACKEND=opengl    # default
#   cmake -B build -DNEXIUM_GRAPHICS_BACKEND=null      # headless / tests

set(NEXIUM_GRAPHICS_BACKEND "opengl" CACHE STRING "Graphics backend: opengl | null")
set_property(CACHE NEXIUM_GRAPHICS_BACKEND PROPERTY STRINGS opengl null)

set(GRAPHICS_ALIAS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/sources/Graphics")
message(STATUS "NEXIUM_GRAPHICS_BACKEND: '${NEXIUM_GRAPHICS_BACKEND}'")

set(GRAPHICS_HEADER_BANNER
    "/* ----------------------------------------------------------- */
/*                                                             */
/*                 THIS FILE IS AUTO GENERATED                 */
/*                                                             */
/* ----------------------------------------------------------- */

#pragma once

"
)

if (NEXIUM_GRAPHICS_BACKEND STREQUAL "opengl")
    set(GRAPHICS_INCLUDE "#include \"GLGraphics.h\"")
    set(GRAPHICS_ALIAS "using Graphics = GLGraphics;")

    set(GRAPHICS_BACKEND_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/sources/Graphics/Implementations/OpenGL/GLGraphics.cpp"
    )
    set(GRAPHICS_BACKEND_LIBS Core::Glad)

elseif (NEXIUM_GRAPHICS_BACKEND STREQUAL "null")
    set(GRAPHICS_INCLUDE "#include \"NullGraphics.hpp\"")
    set(GRAPHICS_ALIAS "using Graphics = NullGraphics;")

    set(GRAPHICS_BACKEND_SOURCES) # header-only
    set(GRAPHICS_BACKEND_LIBS)

else ()
    message(FATAL_ERROR
        "Unknown NEXIUM_GRAPHICS_BACKEND: '${NEXIUM_GRAPHICS_BACKEND}'")
endif ()

file(WRITE "${GRAPHICS_ALIAS_DIR}/Graphics.h"
    "${GRAPHICS_HEADER_BANNER}"
    "${GRAPHICS_INCLUDE}\n\n"
    "${GRAPHICS_ALIAS}\n"
)

# ── Main deps pipeline ────────────────────────────────────────────────────────────────
add_subdirectory(dependencies)
add_subdirectory(sources)
add_subdirectory(tests)

# TODO: in the future it must be removed
add_subdirectory(game-example)
