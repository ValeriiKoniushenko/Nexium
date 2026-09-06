include_guard(GLOBAL)
include(ExternalProject)

# JRM is a build tool. Its executable and parser must never inherit the engine's
# Debug configuration, flags, or link dependencies. The runtime adapter below is
# deliberately built by the engine, so its ABI/runtime matches its consumers.
set(_jrm_source_dir "${PROJECT_SOURCE_DIR}/dependencies/JustReflectMe")
if(NOT EXISTS "${_jrm_source_dir}/sources/JustReflectMe/Adapter.cpp")
    message(FATAL_ERROR
        "JustReflectMe is missing. Run git submodule sync --recursive followed by "
        "git submodule update --init --recursive.")
endif()
if(CMAKE_CROSSCOMPILING)
    message(FATAL_ERROR "The JRM build tool currently requires a native toolchain.")
endif()

set(_jrm_binary_dir "${PROJECT_BINARY_DIR}/tools/jrm")
get_property(_jrm_multi_config GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
if(_jrm_multi_config)
    set(_jrm_executable "${_jrm_binary_dir}/bin/Release/jrm${CMAKE_EXECUTABLE_SUFFIX}")
else()
    set(_jrm_executable "${_jrm_binary_dir}/bin/jrm${CMAKE_EXECUTABLE_SUFFIX}")
endif()

set(_jrm_cmake_args
    "-DCMAKE_BUILD_TYPE:STRING=Release"
    "-DCMAKE_CXX_COMPILER:FILEPATH=${CMAKE_CXX_COMPILER}"
    "-DCMAKE_CXX_FLAGS:STRING="
    "-DCMAKE_CXX_SCAN_FOR_MODULES:BOOL=OFF"
    "-DJRM_DISABLE_TESTS:BOOL=ON"
    "-DJRM_ENABLE_BENCHMARKS:BOOL=OFF"
    "-DJRM_ENABLE_CODE_COVERAGE:BOOL=OFF"
    "-DJRM_GENERATE_CODE_COVERAGE_HTML:BOOL=OFF"
)
if(_jrm_multi_config)
    list(APPEND _jrm_cmake_args "-DCMAKE_CONFIGURATION_TYPES:STRING=Release")
endif()
foreach(_variable IN ITEMS CMAKE_TOOLCHAIN_FILE CMAKE_CXX_COMPILER_LAUNCHER
        CMAKE_MAKE_PROGRAM CMAKE_OSX_ARCHITECTURES CMAKE_OSX_SYSROOT
        CMAKE_OSX_DEPLOYMENT_TARGET)
    if(DEFINED ${_variable})
        string(REPLACE ";" "|" _value "${${_variable}}")
        list(APPEND _jrm_cmake_args "-D${_variable}:STRING=${_value}")
    endif()
endforeach()

ExternalProject_Add(Nexium_JRM_Build
    SOURCE_DIR "${_jrm_source_dir}"
    BINARY_DIR "${_jrm_binary_dir}"
    PREFIX "${PROJECT_BINARY_DIR}/tools/jrm-prefix"
    DOWNLOAD_COMMAND ""
    UPDATE_COMMAND ""
    CMAKE_ARGS ${_jrm_cmake_args}
    LIST_SEPARATOR "|"
    BUILD_COMMAND "${CMAKE_COMMAND}" --build <BINARY_DIR> --config Release --target jrm
    # Check the nested incremental build each time: local edits and submodule
    # revision changes must rebuild JRM, while unchanged sources compile once.
    BUILD_ALWAYS TRUE
    BUILD_BYPRODUCTS "${_jrm_executable}"
    INSTALL_COMMAND ""
    EXCLUDE_FROM_ALL TRUE
)

# Upstream collects translation units with GLOB without CONFIGURE_DEPENDS.
# A submodule update can add/remove a .cpp without changing CMakeLists.txt; make
# that change reconfigure the nested project before its incremental build.
file(GLOB_RECURSE _jrm_sources CONFIGURE_DEPENDS "${_jrm_source_dir}/sources/*.cpp")
file(CONFIGURE OUTPUT "${PROJECT_BINARY_DIR}/tools/jrm-source-files.txt"
    CONTENT "${_jrm_sources}\n" @ONLY
)
ExternalProject_Add_StepDependencies(Nexium_JRM_Build configure
    "${PROJECT_BINARY_DIR}/tools/jrm-source-files.txt"
)

add_executable(JustReflectMe::Generator IMPORTED GLOBAL)
set_target_properties(JustReflectMe::Generator PROPERTIES
    IMPORTED_LOCATION "${_jrm_executable}"
)
add_dependencies(JustReflectMe::Generator Nexium_JRM_Build)

add_subdirectory("${_jrm_source_dir}/dependencies/NlohmannJson"
    "${PROJECT_BINARY_DIR}/dependencies/jrm-json")
add_library(Nexium_JRM_Adapter STATIC
    "${_jrm_source_dir}/sources/JustReflectMe/Adapter.cpp"
    "${_jrm_source_dir}/sources/JustReflectMe/Adapter.h"
)
add_library(JustReflectMe::Adapter ALIAS Nexium_JRM_Adapter)
target_compile_features(Nexium_JRM_Adapter PUBLIC cxx_std_26)
target_include_directories(Nexium_JRM_Adapter SYSTEM PUBLIC "${_jrm_source_dir}/sources")
target_link_libraries(Nexium_JRM_Adapter PUBLIC NlohmannJson)

# Keep JRM's incremental cache, invalidating it when its tool/configuration or
# generated outputs change. Use the imported path, never an unrelated PATH tool.
add_custom_target(CodeGenerator ALL
    COMMAND "${CMAKE_COMMAND}"
        "-DJRM_EXECUTABLE=$<TARGET_FILE:JustReflectMe::Generator>"
        "-DPROJECT_ROOT=${PROJECT_SOURCE_DIR}"
        -P "${CMAKE_CURRENT_LIST_DIR}/RunJRM.cmake"
    DEPENDS JustReflectMe::Generator
    WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
    COMMENT "Generating reflection metadata with Release JRM"
    VERBATIM
)
