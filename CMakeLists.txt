cmake_minimum_required(VERSION 3.30..)

project(Nexium VERSION 1.0 LANGUAGES CXX)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_compile_definitions(NEXIUM_PROJECT_DIR="${CMAKE_CURRENT_SOURCE_DIR}")

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Flags.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Features.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Predefinitions.cmake)

# ========= Use link to the lib, instead of fetching =========
# set(USE_LINK_NO_FETCH__JRM "../../JustReflectMe")

# ========= Base setup & Predefinitions =========
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
#   cmake -B build -DNEXIUM_GRAPHICS_BACKEND=opengl   # default
#   cmake -B build -DNEXIUM_GRAPHICS_BACKEND=null      # headless / tests

set(NEXIUM_GRAPHICS_BACKEND "opengl" CACHE STRING "Graphics backend: opengl | null")
set_property(CACHE NEXIUM_GRAPHICS_BACKEND PROPERTY STRINGS opengl null)

set(GRAPHICS_ALIAS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/sources/Graphics")
message(STATUS "NEXIUM_GRAPHICS_BACKEND: '${NEXIUM_GRAPHICS_BACKEND}'")

if (NEXIUM_GRAPHICS_BACKEND STREQUAL "opengl")
    file(WRITE "${GRAPHICS_ALIAS_DIR}/Graphics.h"
        "#pragma once\n\n"
        "#include \" GLGraphics.h\"\n\n"
        "using Graphics = GLGraphics;\n"
    )
    set(GRAPHICS_BACKEND_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/sources/Graphics/Implementations/OpenGL/GLGraphics.cpp"
    )
    set(GRAPHICS_BACKEND_LIBS Core::Glad)
elseif (NEXIUM_GRAPHICS_BACKEND STREQUAL "null")
    file(WRITE "${GRAPHICS_ALIAS_DIR}/Graphics.h"
        "#pragma once\n"
        "#include \"NullGraphics.hpp\"\n"
        "using Graphics = NullGraphics;\n"
    )
    set(GRAPHICS_BACKEND_SOURCES "")   # NullGraphics is header-only
    set(GRAPHICS_BACKEND_LIBS "")
else ()
    message(FATAL_ERROR "Unknown NEXIUM_GRAPHICS_BACKEND: '${NEXIUM_GRAPHICS_BACKEND}'")
endif ()

# ========= Main deps pipeline =========
add_subdirectory(dependencies)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/FetchDependencies.cmake)

if (DEFINED USE_LINK_NO_FETCH__JRM)
    set(SRC_PATH "${USE_LINK_NO_FETCH__JRM}")
    get_filename_component(LINK_NAME "${SRC_PATH}" NAME)
    set(LINK_PATH "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/${LINK_NAME}")

    if (NOT EXISTS "${LINK_PATH}")
        file(CREATE_LINK
            "${SRC_PATH}"
            "${LINK_PATH}"
            SYMBOLIC
        )
    endif ()

    add_subdirectory("${LINK_PATH}")
endif ()

add_subdirectory(sources)
add_subdirectory(tests)

# TODO: in the future it must be removed
add_subdirectory(game-example)
