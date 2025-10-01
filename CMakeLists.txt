cmake_minimum_required(VERSION 3.30..)

project(Nexium VERSION 1.0 LANGUAGES CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_compile_definitions(DONT_USE_CPP_LIB_STACKTRACE)

add_compile_definitions(NEXIUM_PROJECT_DIR="${CMAKE_CURRENT_SOURCE_DIR}")

include(Flags)
include(Features)
include(Predefinitions)
include(Deploy)

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

# ========= Main deps pipeline =========
add_subdirectory(dependencies)
include(FetchDependencies)

add_subdirectory(sources)
add_subdirectory(tests)

add_subdirectory(game-example)
