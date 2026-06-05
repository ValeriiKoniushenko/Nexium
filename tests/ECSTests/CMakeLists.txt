file(GLOB_RECURSE CPP_SOURCES "*.cpp")
set_source_files_properties(${CPP_SOURCES} PROPERTIES LANGUAGE CXX)

add_executable(ECSTests ${CPP_SOURCES})
target_link_libraries(ECSTests PUBLIC Nexium::Core gtest)
add_dependencies(ECSTests CodeGenerator)
