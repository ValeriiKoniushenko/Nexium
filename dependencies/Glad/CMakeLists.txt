
get_filename_component(MAIN_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
set(TARGET_NAME "__Core_${MAIN_NAME}")

set(SOURCES src/glad.c include/glad/glad.h)
set_source_files_properties(${SOURCES} PROPERTIES LANGUAGE CXX)

add_library(${TARGET_NAME} STATIC ${SOURCES})
add_library(Core::${MAIN_NAME} ALIAS ${TARGET_NAME})

target_include_directories(${TARGET_NAME} PUBLIC include)