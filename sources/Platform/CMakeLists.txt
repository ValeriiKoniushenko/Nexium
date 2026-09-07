# ============== Per Module Private Info ===============

add_library(Nexium_Platform_PrivateModule STATIC)
target_sources(Nexium_Platform_PrivateModule PRIVATE
    PrivateModuleInfo.cpp
    PrivateModuleInfo.h
)
target_link_libraries(Nexium_Platform_PrivateModule PRIVATE Nexium::Foundation)
target_compile_definitions(Nexium_Platform_PrivateModule PRIVATE PRIVATE_MODULE_NAME="Platform")

# ============== Graphics ===============

set(SOURCES
    Graphics.cpp
)

set(HEADERS
    Graphics.h
)

add_library(Nexium_Platform_Graphics STATIC)
add_library(Nexium::Platform::Graphics ALIAS Nexium_Platform_Graphics)
target_sources(Nexium_Platform_Graphics PRIVATE ${SOURCES} ${HEADERS})

target_link_libraries(Nexium_Platform_Graphics
    PUBLIC
    glfw
    Core::Glad
    PRIVATE
    Nexium_Platform_PrivateModule
)

# ================ INPUT =================
set(SOURCES
    Keyboard.cpp
    Mouse.cpp
    Window.cpp
)

set(HEADERS
    Keyboard.h
    Mouse.h
    Window.h
)

add_library(Nexium_Platform_Input STATIC)
add_library(Nexium::Platform::Input ALIAS Nexium_Platform_Input)
target_sources(Nexium_Platform_Input PRIVATE ${SOURCES} ${HEADERS})

target_link_libraries(Nexium_Platform_Input
    PUBLIC
    Nexium::Foundation
    Nexium::Platform::Graphics
    JustReflectMe::Adapter
    glm
    PRIVATE
    Nexium_Platform_PrivateModule
)

# =========================================

nexium_configure_target(Nexium_Platform_PrivateModule)
nexium_configure_target(Nexium_Platform_Graphics)
nexium_configure_target(Nexium_Platform_Input)

add_dependencies(Nexium_Platform_Graphics CodeGenerator)
add_dependencies(Nexium_Platform_Input CodeGenerator)
