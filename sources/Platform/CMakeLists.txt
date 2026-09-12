get_filename_component(MODULE_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
set(MODULE_PRIVATE_TARGET Nexium_${MODULE_NAME}_PrivateModule)

# ============== Per Module Private Info ===============

add_library(${MODULE_PRIVATE_TARGET} STATIC)
target_sources(${MODULE_PRIVATE_TARGET} PRIVATE
    PrivateModuleInfo.cpp
)
target_link_libraries(${MODULE_PRIVATE_TARGET} PRIVATE spdlog::spdlog)
target_compile_definitions(${MODULE_PRIVATE_TARGET} PRIVATE PRIVATE_MODULE_NAME="${MODULE_NAME}")

# ============== Graphics ===============

add_library(Nexium_Platform_Graphics STATIC)
add_library(Nexium::Platform::Graphics ALIAS Nexium_Platform_Graphics)
target_sources(Nexium_Platform_Graphics PRIVATE
    Graphics.cpp
)
nexium_add_public_headers(Nexium_Platform_Graphics
    Platform/Graphics.h
)

target_link_libraries(Nexium_Platform_Graphics
    PUBLIC
    Core::Glad
)

# ================ INPUT =================
add_library(Nexium_Platform_Input STATIC)
add_library(Nexium::Platform::Input ALIAS Nexium_Platform_Input)
target_sources(Nexium_Platform_Input PRIVATE
    Keyboard.cpp
    Mouse.cpp
    Window.cpp
)
nexium_add_public_headers(Nexium_Platform_Input
    Platform/Glfw.h
    Platform/Keyboard.h
    Platform/Keyboard.generated.h
    Platform/Mouse.h
    Platform/Mouse.generated.h
    Platform/Window.h
    Platform/Window.generated.h
)

target_link_libraries(Nexium_Platform_Input
    PUBLIC
    glfw
    Utils::Core
    JustReflectMe::Adapter
    glm
    PRIVATE
    Nexium::Platform::Graphics
    ${MODULE_PRIVATE_TARGET}
)

# =========================================

nexium_configure_target(${MODULE_PRIVATE_TARGET})
nexium_configure_target(Nexium_Platform_Graphics)
nexium_configure_target(Nexium_Platform_Input)

add_dependencies(Nexium_Platform_Graphics CodeGenerator)
add_dependencies(Nexium_Platform_Input CodeGenerator)
