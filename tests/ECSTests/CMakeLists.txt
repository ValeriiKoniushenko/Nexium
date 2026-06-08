file(GLOB CPP_SOURCES "*.cpp")
set_source_files_properties(${CPP_SOURCES} PROPERTIES LANGUAGE CXX)

add_executable(Nexium_ECSTests ${CPP_SOURCES})
target_link_libraries(Nexium_ECSTests PUBLIC Nexium::Core gtest)
add_dependencies(Nexium_ECSTests CodeGenerator)

CoreAddCompileOptionsTo(Nexium_ECSTests)