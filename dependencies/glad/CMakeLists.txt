
set(TARGET_NAME __SW_Glad)

set(SOURCES src/glad.c include/glad/glad.h)
set_source_files_properties(${SOURCES} PROPERTIES LANGUAGE CXX)

add_library(${TARGET_NAME} STATIC ${SOURCES})
add_library(SW::Glad ALIAS ${TARGET_NAME})

target_include_directories(${TARGET_NAME} PUBLIC include)