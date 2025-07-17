add_subdirectory(Camera)
add_subdirectory(Editor)
add_subdirectory(Misc)
add_subdirectory(InputDevices)
add_subdirectory(RawGraphics)
add_subdirectory(SceneObjects)

add_executable(SomeGameApp SomeGameApp.cpp)
target_link_libraries(SomeGameApp PUBLIC
        SW::Camera
        SW::Editor
        SW::InputDevices
)
add_dependencies(SomeGameApp deploy)