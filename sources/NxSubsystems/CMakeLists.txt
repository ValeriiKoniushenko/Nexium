get_filename_component(MODULE_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
set(MODULE_PRIVATE_TARGET Nexium_${MODULE_TARGET}_PrivateModule)

# ============== Per Module Private Info ===============

add_library(${MODULE_PRIVATE_TARGET} STATIC)
target_sources(${MODULE_PRIVATE_TARGET} PRIVATE
    PrivateModuleInfo.cpp
    PrivateModuleInfo.h
)
target_link_libraries(${MODULE_PRIVATE_TARGET} PRIVATE spdlog::spdlog Utils::Core)
target_compile_definitions(${MODULE_PRIVATE_TARGET} PRIVATE PRIVATE_MODULE_NAME="${MODULE_NAME}")

# ================ NxSubsystem::Graphics =================

add_library(Nexium_Subsystem_Graphics STATIC)
add_library(Nexium::Subsystem::Graphics ALIAS Nexium_Subsystem_Graphics)
target_sources(Nexium_Subsystem_Graphics PRIVATE
    Graphics/Debug/Line.h
    Graphics/Debug/Line.cpp

    Graphics/GraphicsComponents.h
    Graphics/GraphicsComponents.cpp
    Graphics/ShaderManager.h
    Graphics/ShaderManager.cpp
    Graphics/ShaderProgram.h
    Graphics/ShaderProgram.cpp
    Graphics/ShaderProgramMeta.h
    Graphics/ShaderProgramMeta.cpp
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

if (TARGET assimp::assimp)
    target_link_libraries(Nexium_Subsystem_Graphics PUBLIC assimp::assimp)
endif ()

# ================ NxSubsystem::AssetsManager =================

add_library(Nexium_Subsystem_AssetsManager STATIC)
add_library(Nexium::Subsystem::AssetsManager ALIAS Nexium_Subsystem_AssetsManager)
target_sources(Nexium_Subsystem_AssetsManager PRIVATE
    AssetsManager/AssetsManager.h
    AssetsManager/AssetsManager.cpp
    AssetsManager/TextureAsset.h
    AssetsManager/TextureAsset.cpp
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
    Input/InputAction.h
    Input/InputAction.cpp
    Input/InputManager.h
    Input/InputManager.cpp
)
target_link_libraries(Nexium_Subsystem_Input 
    PUBLIC 
    Nexium::Foundation
    Nexium::Subsystem::Graphics
    PRIVATE
    ${MODULE_PRIVATE_TARGET}
)

# =========================================

nexium_configure_target(${MODULE_PRIVATE_TARGET})
nexium_configure_target(Nexium_Subsystem_Graphics)
nexium_configure_target(Nexium_Subsystem_AssetsManager)
nexium_configure_target(Nexium_Subsystem_Input)

add_dependencies(Nexium_Subsystem_Graphics CodeGenerator)
add_dependencies(Nexium_Subsystem_AssetsManager CodeGenerator)
add_dependencies(Nexium_Subsystem_Input CodeGenerator)
