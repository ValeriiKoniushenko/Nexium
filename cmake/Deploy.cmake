
function(Nexium_Deploy TARGET_NAME)
    set(_srcAssets "${CMAKE_SOURCE_DIR}/assets")
    set(_srcConfigs "${CMAKE_SOURCE_DIR}/configs")
    set(_srcTestAssets "${CMAKE_SOURCE_DIR}/tests/assets")
    if (WIN32)
        add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
            COMMAND cmd.exe /c "if not exist \"$<TARGET_FILE_DIR:${TARGET_NAME}>/tests\" mkdir \"$<TARGET_FILE_DIR:${TARGET_NAME}>/tests\""
            COMMAND cmd.exe /c "if not exist \"$<TARGET_FILE_DIR:${TARGET_NAME}>/assets\" mklink /J \"$<TARGET_FILE_DIR:${TARGET_NAME}>/assets\" \"${_srcAssets}\""
            COMMAND cmd.exe /c "if not exist \"$<TARGET_FILE_DIR:${TARGET_NAME}>/configs\" mklink /J \"$<TARGET_FILE_DIR:${TARGET_NAME}>/configs\" \"${_srcConfigs}\""
            COMMAND cmd.exe /c "if not exist \"$<TARGET_FILE_DIR:${TARGET_NAME}>/tests/assets\" mklink /J \"$<TARGET_FILE_DIR:${TARGET_NAME}>/tests/assets\" \"${_srcTestAssets}\""
        )
    else ()
        add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${TARGET_NAME}>/tests"
            COMMAND ${CMAKE_COMMAND} -E create_symlink "${_srcAssets}" "$<TARGET_FILE_DIR:${TARGET_NAME}>/assets"
            COMMAND ${CMAKE_COMMAND} -E create_symlink "${_srcConfigs}" "$<TARGET_FILE_DIR:${TARGET_NAME}>/configs"
            COMMAND ${CMAKE_COMMAND} -E create_symlink "${_srcTestAssets}" "$<TARGET_FILE_DIR:${TARGET_NAME}>/tests/assets"
        )
    endif ()
endfunction()
