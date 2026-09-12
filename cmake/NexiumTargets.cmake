include_guard(GLOBAL)

function(nexium_configure_target target)
    cmake_parse_arguments(ARG "PROJECT_DIR" "" "" ${ARGN})

    target_compile_features(${target} PUBLIC cxx_std_26)
    set_target_properties(${target} PROPERTIES CXX_EXTENSIONS OFF)
    target_compile_definitions(${target} PRIVATE "$<$<CONFIG:Debug>:NEXIUM_DEBUG=1>")

    if(ARG_PROJECT_DIR)
        target_compile_definitions(${target} PRIVATE NEXIUM_PROJECT_DIR="${PROJECT_SOURCE_DIR}")
    endif()

    CoreAddCompileOptionsTo(${target})

    if(NEXIUM_VERIFY_INTERFACE_HEADER_SETS)
        get_property(header_sets TARGET ${target} PROPERTY HEADER_SETS)
        if(header_sets)
            set_property(TARGET ${target} PROPERTY VERIFY_INTERFACE_HEADER_SETS ON)
        endif()
    endif()
endfunction()

function(nexium_add_public_headers target)
    set(headers)
    foreach(header IN LISTS ARGN)
        set(header_path "${PROJECT_SOURCE_DIR}/sources/${header}")
        list(APPEND headers "${header_path}")
        if(header MATCHES "\\.generated\\.h$")
            set_source_files_properties(${header_path} PROPERTIES SKIP_LINTING TRUE)
        endif()
    endforeach()

    target_sources(${target}
        PUBLIC
        FILE_SET HEADERS
        BASE_DIRS "${PROJECT_SOURCE_DIR}/sources"
        FILES ${headers}
    )
endfunction()

function(nexium_enable_graphics_debug_api target)
    target_compile_definitions(${target}
        PUBLIC "$<$<CONFIG:Debug>:NEXIUM_GRAPHICS_DEBUG=1>"
    )
endfunction()

function(nexium_enable_debug_api target)
    target_compile_definitions(${target} PUBLIC "$<$<CONFIG:Debug>:NEXIUM_DEBUG=1>")
endfunction()
