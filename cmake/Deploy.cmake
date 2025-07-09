
if(WIN32)
    add_custom_target(copy_assets ALL
            COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets"
            COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/assets/" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets"
            COMMENT "Copying changed assets to ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets"
    )
else()
    add_custom_target(copy_assets ALL
            COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets"
            COMMAND rsync -a --delete --checksum "${CMAKE_SOURCE_DIR}/assets/" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets"
            COMMENT "Copying changed assets to ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets"
    )
endif()

add_custom_target(deploy)
add_dependencies(deploy copy_assets)