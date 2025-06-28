cmake_minimum_required(VERSION 4.0..)

project(SpriteWalker VERSION 1.0 LANGUAGES CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")

include(Flags)
include(Features)
include(Predefinitions)

# ========= Base setup & Predefinitions =========
# CXX standard installing
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(REQUIRED_CXX_STANDARD cxx_std_26)   # Put preferable cxx standard. It will be checked and exported to CXX_STANDARD
set(CXX_STANDARD)                       # Compiler specific value that was going from REQUIRED_CXX_STANDARD
InstallCXXStandardVersion()

# Installing gtest benchmark options
set(BENCHMARK_ENABLE_TESTING)
InstallBenchmarkDependenciesTesting()

# Installing of debug rules
InstallDebugLevel()

# Installing of needed compile data for MSVC
InstallWin32MSVCFlags()

# Misc
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# ========= Main deps pipeline =========
include(FetchDependencies)
add_subdirectory(sources)