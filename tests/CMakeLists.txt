file(GLOB_RECURSE CPP_SOURCES "*.cpp")
set_source_files_properties(${CPP_SOURCES} PROPERTIES LANGUAGE CXX)

add_executable(Nexium_Tests ${CPP_SOURCES})
target_link_libraries(Nexium_Tests PUBLIC Nexium::Core gtest)
add_dependencies(Nexium_Tests CodeGenerator)

CoreAddCompileOptionsTo(Nexium_Tests)
