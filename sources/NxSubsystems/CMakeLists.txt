get_filename_component(MODULE_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
set(MODULE_PRIVATE_TARGET Nexium_${MODULE_NAME}_PrivateModule)

# ============== Per Module Private Info ===============

add_library(${MODULE_PRIVATE_TARGET} STATIC)
target_sources(${MODULE_PRIVATE_TARGET} PRIVATE
    PrivateModuleInfo.cpp
)
target_link_libraries(${MODULE_PRIVATE_TARGET} PRIVATE spdlog::spdlog)
target_compile_definitions(${MODULE_PRIVATE_TARGET} PRIVATE PRIVATE_MODULE_NAME="${MODULE_NAME}")

# ================ NxSubsystem::Graphics =================

add_library(Nexium_Subsystem_Graphics STATIC)
add_library(Nexium::Subsystem::Graphics ALIAS Nexium_Subsystem_Graphics)
target_sources(Nexium_Subsystem_Graphics PRIVATE
    Graphics/Debug/Line.cpp
    Graphics/GraphicsComponents.cpp
    Graphics/ShaderManager.cpp
    Graphics/ShaderProgram.cpp
    Graphics/ShaderProgramMeta.cpp
)
nexium_add_public_headers(Nexium_Subsystem_Graphics
    NxSubsystems/Graphics/Debug/Line.h
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
    Nexium::Platform::Graphics
    Nexium::Fundamental::ResourceManagement
    PRIVATE
    Utils::Utils
    ${MODULE_PRIVATE_TARGET}
)

if(NEXIUM_ENABLE_3D_MODULE)
    add_library(Nexium_Subsystem_AssimpImporter STATIC)
    add_library(Nexium::Subsystem::AssimpImporter ALIAS Nexium_Subsystem_AssimpImporter)
    target_sources(Nexium_Subsystem_AssimpImporter PRIVATE
        Graphics/Import/AssimpMeshImporter.cpp
    )
    nexium_add_public_headers(Nexium_Subsystem_AssimpImporter
        NxSubsystems/Graphics/Import/AssimpMeshImporter.h
    )
    target_link_libraries(Nexium_Subsystem_AssimpImporter
        PUBLIC Nexium::Subsystem::Graphics
        PRIVATE assimp::assimp
    )
endif()

# ================ NxSubsystem::AssetsManager =================

add_library(Nexium_Subsystem_AssetsManager STATIC)
add_library(Nexium::Subsystem::AssetsManager ALIAS Nexium_Subsystem_AssetsManager)
target_sources(Nexium_Subsystem_AssetsManager PRIVATE
    AssetsManager/AssetsManager.cpp
    AssetsManager/TextureAsset.cpp
)
nexium_add_public_headers(Nexium_Subsystem_AssetsManager
    NxSubsystems/AssetsManager/AssetsManager.h
    NxSubsystems/AssetsManager/AssetsManager.generated.h
    NxSubsystems/AssetsManager/TextureAsset.h
    NxSubsystems/AssetsManager/TextureAsset.generated.h
)
target_link_libraries(Nexium_Subsystem_AssetsManager 
    PUBLIC 
    Nexium::Subsystem::Graphics
    Nexium::Fundamental::Assets
    Nexium::Fundamental::TextureAtlas
    Nexium::RawBackend
    PRIVATE
    ${MODULE_PRIVATE_TARGET}
)

# ================ NxSubsystem::Input =================

add_library(Nexium_Subsystem_Input STATIC)
add_library(Nexium::Subsystem::Input ALIAS Nexium_Subsystem_Input)
target_sources(Nexium_Subsystem_Input PRIVATE
    Input/InputAction.cpp
    Input/InputManager.cpp
)
nexium_add_public_headers(Nexium_Subsystem_Input
    NxSubsystems/Input/InputAction.h
    NxSubsystems/Input/InputManager.h
    NxSubsystems/PrivateModuleInfo.h
)
target_link_libraries(Nexium_Subsystem_Input 
    PUBLIC 
    Nexium::Foundation
    Nexium::Platform::Input
    PRIVATE
    ${MODULE_PRIVATE_TARGET}
)

# =========================================

nexium_configure_target(${MODULE_PRIVATE_TARGET})
nexium_configure_target(Nexium_Subsystem_Graphics)
nexium_configure_target(Nexium_Subsystem_AssetsManager)
nexium_configure_target(Nexium_Subsystem_Input)
if(TARGET Nexium_Subsystem_AssimpImporter)
    nexium_configure_target(Nexium_Subsystem_AssimpImporter)
endif()
nexium_enable_graphics_debug_api(Nexium_Subsystem_Graphics)

add_dependencies(Nexium_Subsystem_Graphics CodeGenerator)
add_dependencies(Nexium_Subsystem_AssetsManager CodeGenerator)
add_dependencies(Nexium_Subsystem_Input CodeGenerator)
if(TARGET Nexium_Subsystem_AssimpImporter)
    add_dependencies(Nexium_Subsystem_AssimpImporter CodeGenerator)
endif()
