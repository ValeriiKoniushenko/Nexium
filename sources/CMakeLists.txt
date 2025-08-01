add_subdirectory(Camera)
add_subdirectory(Editor)
add_subdirectory(GameplaySystem)
add_subdirectory(Misc)
add_subdirectory(InputDevices)
add_subdirectory(Graphics)

add_library(SpriteWalkerEngine INTERFACE)

target_link_libraries(SpriteWalkerEngine INTERFACE
    assimp::assimp
    SW::Camera
    SW::Editor
    SW::GameplaySystem
    SW::Misc
    SW::InputDevices
    SW::Graphics
)

target_include_directories(SpriteWalkerEngine INTERFACE ./)

add_executable(SomeGameApp SomeGameApp.cpp)

target_link_libraries(SomeGameApp PUBLIC
    SpriteWalkerEngine
    assimp
)
