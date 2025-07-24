add_subdirectory(Camera)
add_subdirectory(Editor)
add_subdirectory(GameplaySystem)
add_subdirectory(Misc)
add_subdirectory(InputDevices)
add_subdirectory(RawGraphics)
add_subdirectory(SceneObjects)

add_library(SpriteWalkerEngine INTERFACE)

target_link_libraries(SpriteWalkerEngine INTERFACE
    SW::Camera
    SW::Editor
    SW::InputDevices
    assimp::assimp
)

target_include_directories(SpriteWalkerEngine INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(SomeGameApp SomeGameApp.cpp)

target_link_libraries(SomeGameApp PUBLIC
        SW::Camera
        SW::Editor
        SW::InputDevices
)
