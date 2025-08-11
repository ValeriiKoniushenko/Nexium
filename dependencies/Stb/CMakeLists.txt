
get_filename_component(MAIN_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
set(TARGET_NAME "__Core_${MAIN_NAME}")

add_library(${TARGET_NAME} INTERFACE)
target_sources(${TARGET_NAME} PRIVATE Image.h)

add_library(Core::${MAIN_NAME} ALIAS ${TARGET_NAME})

target_include_directories(${TARGET_NAME} INTERFACE ../)