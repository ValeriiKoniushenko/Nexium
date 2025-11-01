
file(GLOB_RECURSE CPP_SOURCES "*.cpp")
set_source_files_properties(${CPP_SOURCES} PROPERTIES LANGUAGE CXX)

file(GLOB_RECURSE HEADERS "*.h")

add_executable(EngineTests ${CPP_SOURCES} ${HEADERS})

target_link_libraries(EngineTests PUBLIC
    gtest
    Core::AssetsManager
    Core::Camera
    Core::Editor
    Core::GameplaySystem
    Core::Misc
    Core::InputDevices
    Core::Graphics
    Core::Scene
    assimp::assimp
)

CoreAddCompileOptionsTo(EngineTests)
