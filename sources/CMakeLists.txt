add_subdirectory(Camera)
add_subdirectory(Editor)
add_subdirectory(GameplaySystem)
add_subdirectory(Misc)
add_subdirectory(InputDevices)
add_subdirectory(Graphics)

add_library(SpriteWalkerEngine INTERFACE)

target_link_libraries(SpriteWalkerEngine INTERFACE
    SW::Camera
    SW::GameplaySystem
    SW::Misc
    SW::InputDevices
    SW::Graphics
    SW::ImGui
    assimp
)

target_include_directories(SpriteWalkerEngine INTERFACE ./)