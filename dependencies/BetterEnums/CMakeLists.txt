
get_filename_component(MAIN_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
set(TARGET_NAME "__SW_${MAIN_NAME}")

set(SOURCES enum.h)
set_source_files_properties(${SOURCES} PROPERTIES LANGUAGE CXX)

add_library(${TARGET_NAME} INTERFACE ${SOURCES})
add_library(SW::${MAIN_NAME} ALIAS ${TARGET_NAME})

target_include_directories(${TARGET_NAME} INTERFACE ./)