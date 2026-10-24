add_library(Nexium_Subsystem_Graphics STATIC)
add_library(Nexium::Subsystem::Graphics ALIAS Nexium_Subsystem_Graphics)
target_sources(Nexium_Subsystem_Graphics PRIVATE
    Debug/Line.cpp
    Font.cpp
    GraphicsComponents.cpp
    ShaderManager.cpp
    ShaderProgram.cpp
    ShaderProgramMeta.cpp
)
nexium_add_public_headers(Nexium_Subsystem_Graphics
    NxSubsystems/Graphics/Debug/Line.h
    NxSubsystems/Graphics/Font.h
    NxSubsystems/Graphics/GraphicsComponents.h
    NxSubsystems/Graphics/GraphicsComponents.generated.h
    NxSubsystems/Graphics/ShaderManager.h
    NxSubsystems/Graphics/ShaderProgram.h
    NxSubsystems/Graphics/ShaderProgramMeta.h
    NxSubsystems/Graphics/ShaderProgramMeta.generated.h
)

target_link_libraries(Nexium_Subsystem_Graphics
    PUBLIC
    Nexium::Foundation
    Nexium::Foundation::Interfaces
    Nexium::Fundamental::ResourceManagement
    JustReflectMe::Adapter
    Nexium::Platform::Graphics
    Nexium::RawBackend::Graphics
    Utils::Core
    glm
    PRIVATE
    Nexium::RawBackend::Fonts
    Utils::Utils
    spdlog::spdlog
    ${MODULE_PRIVATE_TARGET}
)

nexium_configure_target(Nexium_Subsystem_Graphics)
nexium_enable_graphics_debug_api(Nexium_Subsystem_Graphics)
add_dependencies(Nexium_Subsystem_Graphics CodeGenerator)

if(NEXIUM_ENABLE_3D_MODULE)
    add_subdirectory(Import)
endif()
