
if(WIN32)
    file(TO_NATIVE_PATH "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets" _dstAssets)
    file(TO_NATIVE_PATH "${CMAKE_SOURCE_DIR}/assets/" _srcAssets)
    execute_process(COMMAND cmd.exe /c if not exist "${_dstAssets}" mklink /J "${_dstAssets}" "${_srcAssets}")

    file(TO_NATIVE_PATH "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/editor" _dstEditor)
    file(TO_NATIVE_PATH "${CMAKE_SOURCE_DIR}/editor/" _srcEditor)
    execute_process(COMMAND cmd.exe /c if not exist "${_dstEditor}" mklink /J "${_dstEditor}" "${_srcEditor}")

    file(MAKE_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/tests")
    file(TO_NATIVE_PATH "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/tests/assets" _dstTestAssets)
    file(TO_NATIVE_PATH "${CMAKE_SOURCE_DIR}/tests/assets/" _srcTestAssets)
    execute_process(COMMAND cmd.exe /c if not exist "${_dstTestAssets}" mklink /J "${_dstTestAssets}" "${_srcTestAssets}")
else()
    add_custom_target(symlinks_to_engine_assets ALL
            COMMAND ${CMAKE_COMMAND} -E create_symlink "${CMAKE_SOURCE_DIR}/assets/" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets"
            COMMAND ${CMAKE_COMMAND} -E create_symlink "${CMAKE_SOURCE_DIR}/editor/" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/editor"
    )
    add_custom_target(deploy_engine)
    add_dependencies(deploy_engine symlinks_to_engine_assets)

    add_custom_target(symlinks_to_tests_assets ALL
        COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/tests"
        COMMAND ${CMAKE_COMMAND} -E create_symlink "${CMAKE_SOURCE_DIR}/tests/assets/" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/tests/assets"
    )

    add_custom_target(deploy_tests)
    add_dependencies(deploy_tests symlinks_to_tests_assets)
endif ()
