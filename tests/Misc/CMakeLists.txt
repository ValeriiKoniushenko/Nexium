file(GLOB CPP_SOURCES "*.cpp")
set_source_files_properties(${CPP_SOURCES} PROPERTIES LANGUAGE CXX)

add_executable(Nexium_MiscTests ${CPP_SOURCES})
target_link_libraries(Nexium_MiscTests PUBLIC Nexium::Core gtest)
add_dependencies(Nexium_MiscTests CodeGenerator)

CoreAddCompileOptionsTo(Nexium_MiscTests)