add_subdirectory(AssetsManager)
add_subdirectory(Camera)
add_subdirectory(Editor)
add_subdirectory(GameplaySystem)
add_subdirectory(Misc)
add_subdirectory(InputDevices)
add_subdirectory(Graphics)
add_subdirectory(Scene)

add_library(NexiumEngine INTERFACE)

target_link_libraries(NexiumEngine INTERFACE
    Core::AssetsManager
    Core::Camera
    Core::Editor
    Core::GameplaySystem
    Core::Misc
    Core::InputDevices
    Core::Graphics
    Core::ImGui
    assimp
)

target_include_directories(NexiumEngine INTERFACE ./)