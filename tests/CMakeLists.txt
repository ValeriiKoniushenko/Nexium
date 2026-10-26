file(GLOB_RECURSE CPP_SOURCES "*.cpp")
set_source_files_properties(${CPP_SOURCES} PROPERTIES LANGUAGE CXX)

add_executable(Nexium_Tests ${CPP_SOURCES})
target_precompile_headers(Nexium_Tests PRIVATE TestPCH.h)
target_include_directories(Nexium_Tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(Nexium_Tests PRIVATE
    gtest
    Nexium::Fundamental::Base
    Nexium::Fundamental::ECS
    Nexium::Subsystem::AssetsManager
    Nexium::Runtime::Base
    Nexium::World::Animation
    Nexium::Editor::Base
    imguizmo::imguizmo
)
add_dependencies(Nexium_Tests CodeGenerator)

nexium_configure_target(Nexium_Tests)
