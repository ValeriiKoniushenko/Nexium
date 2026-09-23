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
        if(header MATCHES "\\.generated\\.h$")
            # JRM creates these during the build, after CMake configuration.
            if(NOT EXISTS "${header_path}")
                continue()
            endif()
            set_source_files_properties(${header_path} PROPERTIES SKIP_LINTING TRUE)
        endif()
        list(APPEND headers "${header_path}")
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

function(nexium_configure_application target)
    cmake_parse_arguments(ARG "" "APPLICATION_ID;DISPLAY_NAME" "" ${ARGN})

    if(NOT ARG_DISPLAY_NAME)
        set(ARG_DISPLAY_NAME "${target}")
    endif()
    if(NOT ARG_APPLICATION_ID)
        set(ARG_APPLICATION_ID "${target}")
    endif()

    set(nexium_root "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/..")

    if(WIN32)
        target_sources(${target} PRIVATE
            "${nexium_root}/sources/Platform/Resources/app.rc"
        )
    elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
        set(icon_source "${nexium_root}/data/internal/logo_192.png")
        set(icon_output "$<TARGET_FILE_DIR:${target}>/${ARG_APPLICATION_ID}.png")
        set(desktop_output "$<TARGET_FILE_DIR:${target}>/${ARG_APPLICATION_ID}.desktop")

        add_custom_target(${target}_DesktopLauncher ALL
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                "${icon_source}" "${icon_output}"
            COMMAND "${CMAKE_COMMAND}"
                "-DNEXIUM_DESKTOP_FILE=${desktop_output}"
                "-DNEXIUM_DESKTOP_EXECUTABLE=$<TARGET_FILE:${target}>"
                "-DNEXIUM_DESKTOP_ICON=${icon_output}"
                "-DNEXIUM_DESKTOP_NAME=${ARG_DISPLAY_NAME}"
                "-DNEXIUM_DESKTOP_WM_CLASS=${ARG_APPLICATION_ID}"
                "-DNEXIUM_DESKTOP_WORKING_DIRECTORY=${PROJECT_SOURCE_DIR}"
                -P "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/WriteDesktopEntry.cmake"
            DEPENDS
                ${target}
                "${icon_source}"
                "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/WriteDesktopEntry.cmake"
            COMMENT "Creating ${ARG_DISPLAY_NAME} desktop launcher"
            VERBATIM
        )

        find_program(xdg_desktop_menu xdg-desktop-menu)
        if(xdg_desktop_menu)
            add_custom_target(${target}_RegisterDesktopLauncher
                COMMAND "${xdg_desktop_menu}" install --mode user --novendor
                    "${desktop_output}"
                DEPENDS ${target}_DesktopLauncher
                COMMENT "Registering ${ARG_DISPLAY_NAME} with the desktop environment"
                VERBATIM
            )
        endif()
    endif()
endfunction()
