add_subdirectory(Camera)
add_subdirectory(GameplaySystem)
add_subdirectory(Misc)
add_subdirectory(InputDevices)
add_subdirectory(Graphics)

add_library(SpriteWalkerEngine INTERFACE)

target_link_libraries(SpriteWalkerEngine INTERFACE
    assimp::assimp
    SW::Camera
    SW::GameplaySystem
    SW::Misc
    SW::InputDevices
    SW::Graphics
    SW::ImGui
)

target_include_directories(SpriteWalkerEngine INTERFACE ./)

add_executable(SomeGameApp SomeGameApp.cpp)

target_link_libraries(SomeGameApp PUBLIC
    SpriteWalkerEngine
    assimp
)

target_compile_options(SomeGameApp PRIVATE
    $<$<CONFIG:Release>:-O3>
    $<$<CONFIG:Debug>:-g3 -O0 -fno-inline>
)
