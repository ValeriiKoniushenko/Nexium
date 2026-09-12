file(GLOB_RECURSE CPP_SOURCES "*.cpp")
set_source_files_properties(${CPP_SOURCES} PROPERTIES LANGUAGE CXX)

add_executable(Nexium_Tests ${CPP_SOURCES})
target_link_libraries(Nexium_Tests PRIVATE
    gtest
    Nexium::Fundamental::Base
    Nexium::Fundamental::ECS
    Nexium::Subsystem::AssetsManager
    Nexium::World::Animation
)
add_dependencies(Nexium_Tests CodeGenerator)

nexium_configure_target(Nexium_Tests)
