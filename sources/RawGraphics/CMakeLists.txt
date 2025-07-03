
get_filename_component(MAIN_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
set(TARGET_NAME "__SW_${MAIN_NAME}")

file(GLOB_RECURSE SOURCES
        "*.h"
        "*.cpp"
)
set_source_files_properties(${SOURCES} PROPERTIES LANGUAGE CXX)

add_library(${TARGET_NAME} STATIC ${SOURCES})
add_library(SW::${MAIN_NAME} ALIAS ${TARGET_NAME})

target_link_libraries(${TARGET_NAME} PUBLIC Utils::Utils SW::Glad glfw SW::BetterEnums)

target_include_directories(${TARGET_NAME} PUBLIC ../)