
if(WIN32)
    add_custom_target(symlinks_to_assets ALL
            COMMAND ${CMAKE_COMMAND} -E create_symlink --directory "${CMAKE_SOURCE_DIR}/assets/" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets"
            COMMAND ${CMAKE_COMMAND} -E create_symlink --directory "${CMAKE_SOURCE_DIR}/editor/" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/editor"
    )
else()
    add_custom_target(symlinks_to_assets ALL
            COMMAND ${CMAKE_COMMAND} -E create_symlink "${CMAKE_SOURCE_DIR}/assets/" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets"
            COMMAND ${CMAKE_COMMAND} -E create_symlink "${CMAKE_SOURCE_DIR}/editor/" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/editor"
    )
endif()

add_custom_target(deploy)
add_dependencies(deploy symlinks_to_assets)