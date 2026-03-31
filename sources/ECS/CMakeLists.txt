get_filename_component(CURRENT_DIR_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
set(TARGET_NAME "__${CURRENT_DIR_NAME}")
set(CLEAR_TARGET_NAME "${CURRENT_DIR_NAME}")

file(GLOB_RECURSE CPP_SOURCES "*.cpp")
set_source_files_properties(${CPP_SOURCES} PROPERTIES LANGUAGE CXX)

file(GLOB_RECURSE CPP_HEADERS "*.h")
exclude_generated_files(FILTERED_HEADERS CPP_HEADERS)

add_library(${TARGET_NAME} STATIC)
target_sources(${TARGET_NAME}
    PRIVATE
    ${CPP_SOURCES}
    PUBLIC
    ${FILTERED_HEADERS}
)

add_dependencies(${TARGET_NAME} CodeGenerator)

add_library(Nexium::${CLEAR_TARGET_NAME} ALIAS ${TARGET_NAME})

target_include_directories(${TARGET_NAME} PUBLIC ..)

target_link_libraries(${TARGET_NAME} PUBLIC
    Utils::Core
    Nexium::RawDataManagement
)

CoreAddCompileOptionsTo(${TARGET_NAME})
