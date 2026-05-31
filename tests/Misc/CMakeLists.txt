file(GLOB_RECURSE CPP_SOURCES "*.cpp")
set_source_files_properties(${CPP_SOURCES} PROPERTIES LANGUAGE CXX)

add_executable(NexiumMiscTests ${CPP_SOURCES})
target_link_libraries(NexiumMiscTests PUBLIC Nexium::Core gtest)
add_dependencies(NexiumMiscTests CodeGenerator)
