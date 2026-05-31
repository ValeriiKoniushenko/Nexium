file(GLOB_RECURSE CPP_SOURCES "*.cpp")
set_source_files_properties(${CPP_SOURCES} PROPERTIES LANGUAGE CXX)

add_executable(GraphicsTests ${CPP_SOURCES})
target_link_libraries(GraphicsTests PUBLIC Nexium::Core gtest)
add_dependencies(GraphicsTests CodeGenerator)
