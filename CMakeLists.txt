cmake_minimum_required(VERSION 3.30)

if(MSVC)
    # Use /Z7 so debug builds remain compatible with ccache.
    set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT
        "$<$<CONFIG:Debug,RelWithDebInfo>:Embedded>"
    )
endif()

project(Nexium VERSION 1.0 LANGUAGES CXX)

# The nested Agents repository validates Git symlink support and links its shared
# skills into .agents/skills without overwriting project-local skills.
include("${CMAKE_CURRENT_SOURCE_DIR}/.agents/Agents/CMakeLists.txt")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux" AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    find_program(MOLD_LINKER mold)
    find_program(LLD_LINKER ld.lld)

    if(MOLD_LINKER)
        message(STATUS "Using mold linker")
        add_link_options(-fuse-ld=mold)
    elseif(LLD_LINKER)
        message(STATUS "Using lld linker")
        add_link_options(-fuse-ld=lld)
    else()
        message(STATUS "Using default system linker")
    endif()
endif()

if(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
    find_program(CCACHE_PROGRAM ccache)
    if(CCACHE_PROGRAM AND NOT DEFINED CMAKE_C_COMPILER_LAUNCHER)
        set(CMAKE_C_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
    endif()
    if(CCACHE_PROGRAM AND NOT DEFINED CMAKE_CXX_COMPILER_LAUNCHER)
        set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
        message(STATUS "ccache: ${CCACHE_PROGRAM}")
    endif()
endif()

set(CMAKE_CXX_SCAN_FOR_MODULES OFF)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/Flags.cmake")
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/Features.cmake")
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/NexiumTargets.cmake")

option(NEXIUM_DISABLE_TESTS "Disable Nexium unit-tests targets" ON)
option(NEXIUM_DISABLE_BENCHMARKS "Disable Nexium benchmark targets" ON)
option(NEXIUM_ENABLE_3D_MODULE "Enable Nexium 3D Module" OFF)
option(NEXIUM_VERIFY_INTERFACE_HEADER_SETS "Create public-header verification targets" ON)
option(NEXIUM_ENABLE_LAYER_CHECKS "Create CMake layer consumer checks" ON)

# ── Base setup & Predefinitions ────────────────────────────────────────────────
# CXX standard installing
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(REQUIRED_CXX_STANDARD cxx_std_26) # Put preferable cxx standard. It will be checked and exported to CXX_STANDARD
set(CXX_STANDARD) # Compiler specific value that was going from REQUIRED_CXX_STANDARD
InstallCXXStandardVersion()

set(BENCHMARK_ENABLE_TESTING OFF)

# ── Main deps pipeline ────────────────────────────────────────────────────────────────
# Configure vendored targets, but build only those required by active layers.
add_subdirectory(dependencies EXCLUDE_FROM_ALL)

if(NEXIUM_ENABLE_3D_MODULE AND NOT TARGET assimp::assimp)
    message(FATAL_ERROR
        "NEXIUM_ENABLE_3D_MODULE=ON requires the vendored assimp::assimp target. "
        "Initialize the assimp dependency or configure with NEXIUM_ENABLE_3D_MODULE=OFF."
    )
endif()

add_subdirectory(sources)

if(NOT NEXIUM_DISABLE_TESTS)
    add_subdirectory(tests)
endif()

if(NOT NEXIUM_DISABLE_BENCHMARKS)
    add_subdirectory(benchmarks)
endif()

if(NEXIUM_ENABLE_LAYER_CHECKS)
    include("${CMAKE_CURRENT_SOURCE_DIR}/tests/cmake/CheckLayers.cmake")
endif()

# TODO: in the future it must be removed
# add_subdirectory(game-example)
