add_library(Nexium_Platform_Input STATIC)
add_library(Nexium::Platform::Input ALIAS Nexium_Platform_Input)
target_sources(Nexium_Platform_Input PRIVATE
    Keyboard.cpp
    Mouse.cpp
    Window.cpp
)
nexium_add_public_headers(Nexium_Platform_Input
    Platform/Input/Glfw.h
    Platform/Input/Keyboard.h
    Platform/Input/Keyboard.generated.h
    Platform/Input/Mouse.h
    Platform/Input/Mouse.generated.h
    Platform/Input/Window.h
    Platform/Input/Window.generated.h
)

target_link_libraries(Nexium_Platform_Input
    PUBLIC
    Nexium::Foundation::Base
    Nexium::Foundation::Interfaces
    JustReflectMe::Adapter
    Utils::Core
    glfw
    glm
    PRIVATE
    Core::Stb
    Nexium::Platform::Graphics
    Nexium::Platform::Resources
    ${MODULE_PRIVATE_TARGET}
)

nexium_configure_target(Nexium_Platform_Input)
add_dependencies(Nexium_Platform_Input CodeGenerator)
