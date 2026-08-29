cmake_minimum_required(VERSION 3.30..)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (NOT DEFINED NEXIUM_LINKER_CONFIGURED)
    set(NEXIUM_LINKER_CONFIGURED TRUE)

    find_program(MOLD_LINKER mold)
    find_program(LLD_LINKER ld.lld)

    if (MOLD_LINKER)
        message(STATUS "Using mold linker")
        add_link_options(-fuse-ld=mold)
    elseif (LLD_LINKER)
        message(STATUS "Using lld linker")
        add_link_options(-fuse-ld=lld)
    else ()
        message(STATUS "Using default system linker")
    endif ()
endif ()

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

# ── Main deps pipeline ────────────────────────────────────────────────────────────────
add_subdirectory(dependencies)
add_subdirectory(sources)

if (NOT NEXIUM_DISABLE_TESTS)
    add_subdirectory(tests)
endif ()

option(NEXIUM_DISABLE_BENCHMARKS "Disable Nexium benchmark targets" OFF)
if (NOT NEXIUM_DISABLE_BENCHMARKS)
    add_subdirectory(benchmarks)
endif ()

# TODO: in the future it must be removed
add_subdirectory(game-example)
