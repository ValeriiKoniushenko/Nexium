
if(WIN32)
    file(TO_NATIVE_PATH "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets" _dstDir)
    file(TO_NATIVE_PATH "${CMAKE_SOURCE_DIR}/assets/" _srcDir)
    execute_process(COMMAND cmd.exe /c mklink /J "${_dstDir}" "${_srcDir}")

    file(TO_NATIVE_PATH "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/editor" _dstDir)
    file(TO_NATIVE_PATH "${CMAKE_SOURCE_DIR}/editor/" _srcDir)
    execute_process(COMMAND cmd.exe /c mklink /J "${_dstDir}" "${_srcDir}")
else()
    add_custom_target(symlinks_to_assets ALL
            COMMAND ${CMAKE_COMMAND} -E create_symlink "${CMAKE_SOURCE_DIR}/assets/" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets"
            COMMAND ${CMAKE_COMMAND} -E create_symlink "${CMAKE_SOURCE_DIR}/editor/" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/editor"
    )
    add_custom_target(deploy)
    add_dependencies(deploy symlinks_to_assets)
endif()

