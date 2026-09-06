cmake_minimum_required(VERSION 3.30)

if(NOT EXISTS "${JRM_EXECUTABLE}" OR NOT IS_DIRECTORY "${PROJECT_ROOT}/sources")
    message(FATAL_ERROR "RunJRM requires a built JRM_EXECUTABLE and a PROJECT_ROOT with sources/.")
endif()

# Multiple engine build directories share generated headers and JRM's cache.
# Serialize writers and share the invalidation state with those generated files.
set(_state_dir "${PROJECT_ROOT}/.cache/nexium-codegen")
file(MAKE_DIRECTORY "${_state_dir}")
file(LOCK "${_state_dir}/lock" GUARD PROCESS TIMEOUT 120)

file(SHA256 "${JRM_EXECUTABLE}" _tool_hash)
set(_config_hash "")
if(EXISTS "${PROJECT_ROOT}/.jrm/config.yaml")
    file(SHA256 "${PROJECT_ROOT}/.jrm/config.yaml" _config_hash)
endif()
set(_signature "${_tool_hash}\n${_config_hash}\n")
set(_previous_signature "")
if(EXISTS "${_state_dir}/signature")
    file(READ "${_state_dir}/signature" _previous_signature)
endif()

set(_invalidate FALSE)
if(NOT _signature STREQUAL _previous_signature)
    set(_invalidate TRUE)
endif()
if(EXISTS "${_state_dir}/outputs")
    file(STRINGS "${_state_dir}/outputs" _outputs)
    foreach(_output IN LISTS _outputs)
        if(NOT EXISTS "${PROJECT_ROOT}/${_output}")
            set(_invalidate TRUE)
            break()
        endif()
    endforeach()
else()
    set(_invalidate TRUE)
endif()
if(_invalidate)
    # JRM's cache tracks input timestamps, not missing outputs or config/tool
    # changes. Removing this disposable cache forces it to regenerate metadata.
    file(REMOVE "${PROJECT_ROOT}/.jrm/cache.data")
endif()

execute_process(
    COMMAND "${JRM_EXECUTABLE}" "${PROJECT_ROOT}"
    WORKING_DIRECTORY "${PROJECT_ROOT}"
    RESULT_VARIABLE _result
)
if(NOT _result STREQUAL "0")
    # A failed generator may already have updated part of its input cache.
    file(REMOVE "${PROJECT_ROOT}/.jrm/cache.data" "${_state_dir}/signature")
    message(FATAL_ERROR "JustReflectMe failed: ${_result}")
endif()

file(GLOB_RECURSE _outputs RELATIVE "${PROJECT_ROOT}"
    "${PROJECT_ROOT}/sources/*.generated.h"
    "${PROJECT_ROOT}/sources/*.generated.inl"
    "${PROJECT_ROOT}/sources/*.generated.cpp"
)
list(JOIN _outputs "\n" _output_manifest)
file(WRITE "${_state_dir}/outputs" "${_output_manifest}\n")
file(WRITE "${_state_dir}/signature" "${_signature}")
