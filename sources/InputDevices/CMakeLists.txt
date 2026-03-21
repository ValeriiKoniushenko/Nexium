get_filename_component(CURRENT_DIR_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
set(TARGET_NAME "__${CURRENT_DIR_NAME}")
set(CLEAR_TARGET_NAME "${CURRENT_DIR_NAME}")

file(GLOB_RECURSE CPP_SOURCES "*.cpp")
set_source_files_properties(${CPP_SOURCES} PROPERTIES LANGUAGE CXX)

file(GLOB_RECURSE HEADERS "*.h")

add_library(${TARGET_NAME} STATIC ${CPP_SOURCES} ${HEADERS})
add_library(Nexium::${CLEAR_TARGET_NAME} ALIAS ${TARGET_NAME})

target_include_directories(${TARGET_NAME} PUBLIC ..)

target_link_libraries(${TARGET_NAME} PUBLIC
    Utils::Core
    glfw
    Nexium::Misc
)

CoreAddCompileOptionsTo(${TARGET_NAME})
