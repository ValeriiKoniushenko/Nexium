get_filename_component(MODULE_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
set(MODULE_TARGET Nexium_${MODULE_NAME})
set(MODULE_PRIVATE_TARGET ${MODULE_TARGET}_PrivateModule)
set(MODULE_ALIAS Nexium::${MODULE_NAME})

# ============== Per Module Private Info ===============

add_library(${MODULE_PRIVATE_TARGET} STATIC)
target_sources(${MODULE_PRIVATE_TARGET} PRIVATE
    PrivateModuleInfo.cpp
    PrivateModuleInfo.h
)
target_link_libraries(${MODULE_PRIVATE_TARGET} PRIVATE Nexium::Foundation)
target_compile_definitions(${MODULE_PRIVATE_TARGET} PRIVATE PRIVATE_MODULE_NAME="${MODULE_NAME}")

# ================ NxSubsystem::Graphics =================

add_library(Nexium_Subsystem_Graphics STATIC)
add_library(Nexium::Subsystem::Graphics ALIAS Nexium_Subsystem_Graphics)
target_sources(Nexium_Subsystem_Graphics PRIVATE
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
    PRIVATE
    ${MODULE_PRIVATE_TARGET}
)

if (TARGET assimp::assimp)
    target_link_libraries(Nexium_Subsystem_Graphics PUBLIC assimp::assimp)
endif ()

# ================ NxSubsystem::AssetsManager =================

add_library(Nexium_Subsystem_AssetsManager STATIC)
add_library(Nexium::Subsystem::AssetsManager ALIAS Nexium_Subsystem_AssetsManager)
target_sources(Nexium_Subsystem_AssetsManager PRIVATE
    AssetsManager/AssetsManager.cpp
    AssetsManager/AssetsManager.h
)
target_link_libraries(Nexium_Subsystem_AssetsManager 
    PUBLIC 
    Nexium::Subsystem::Graphics
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
