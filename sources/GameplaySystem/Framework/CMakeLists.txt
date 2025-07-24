
get_filename_component(PARENT_DIR ${CMAKE_CURRENT_SOURCE_DIR} DIRECTORY)
get_filename_component(PARENT_NAME ${PARENT_DIR} NAME)

get_filename_component(MAIN_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
set(TARGET_NAME "__SW_${PARENT_NAME}_${MAIN_NAME}")

file(GLOB_RECURSE CPP_SOURCES "*.cpp")
set_source_files_properties(${CPP_SOURCES} PROPERTIES LANGUAGE CXX)

file(GLOB_RECURSE HEADERS "*.h")

add_library(${TARGET_NAME} STATIC ${CPP_SOURCES} ${HEADERS})

add_library(SW::${PARENT_NAME}::${MAIN_NAME} ALIAS ${TARGET_NAME})

target_link_libraries(${TARGET_NAME} PUBLIC SW::Misc)

target_include_directories(${TARGET_NAME} PUBLIC ../)