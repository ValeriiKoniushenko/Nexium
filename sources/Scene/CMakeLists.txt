
get_filename_component(MAIN_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
set(TARGET_NAME "__Core_${MAIN_NAME}")

file(GLOB_RECURSE CPP_SOURCES "*.cpp")
set_source_files_properties(${CPP_SOURCES} PROPERTIES LANGUAGE CXX)

file(GLOB_RECURSE HEADERS "*.h")

add_library(${TARGET_NAME} STATIC ${CPP_SOURCES} ${HEADERS})
add_library(Core::${MAIN_NAME} ALIAS ${TARGET_NAME})

target_link_libraries(${TARGET_NAME} PUBLIC
    Core::Graphics
    Boost::smart_ptr
)

target_include_directories(${TARGET_NAME} PUBLIC ../)

CoreAddCompileOptionsTo(${TARGET_NAME})