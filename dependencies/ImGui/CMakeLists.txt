
get_filename_component(MAIN_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
set(TARGET_NAME "__SW_${MAIN_NAME}")

file(GLOB CPP_SOURCES "*.cpp")
list(APPEND CPP_SOURCES backends/imgui_impl_opengl3.cpp backends/imgui_impl_glfw.cpp)
set_source_files_properties(${CPP_SOURCES} PROPERTIES LANGUAGE CXX)

file(GLOB HEADERS "*.h")

add_library(${TARGET_NAME} STATIC ${CPP_SOURCES} ${HEADERS})
add_library(SW::${MAIN_NAME} ALIAS ${TARGET_NAME})

target_include_directories(${TARGET_NAME} PUBLIC ../)

target_link_libraries(${TARGET_NAME} PUBLIC glfw)

target_compile_options(${TARGET_NAME} PRIVATE
    $<$<CONFIG:Release>:-O3>
    $<$<CONFIG:Debug>:-g3 -O0 -fno-inline>
)
