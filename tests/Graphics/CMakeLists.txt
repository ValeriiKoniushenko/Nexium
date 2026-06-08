file(GLOB CPP_SOURCES "*.cpp")
set_source_files_properties(${CPP_SOURCES} PROPERTIES LANGUAGE CXX)

add_executable(Nexium_GraphicsTests ${CPP_SOURCES})
target_link_libraries(Nexium_GraphicsTests PUBLIC Nexium::Core gtest)
add_dependencies(Nexium_GraphicsTests CodeGenerator)

CoreAddCompileOptionsTo(Nexium_GraphicsTests)