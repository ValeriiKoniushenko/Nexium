get_filename_component(MODULE_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
set(MODULE_PRIVATE_TARGET Nexium_${MODULE_NAME}_PrivateModule)

# ============== Per Module Private Info ===============

add_library(${MODULE_PRIVATE_TARGET} STATIC)
target_sources(${MODULE_PRIVATE_TARGET} PRIVATE
    PrivateModuleInfo.cpp
    PrivateModuleInfo.h
)
target_link_libraries(${MODULE_PRIVATE_TARGET} PRIVATE spdlog::spdlog)
target_compile_definitions(${MODULE_PRIVATE_TARGET} PRIVATE PRIVATE_MODULE_NAME="${MODULE_NAME}")

# ============== Sub-libraries ===============

add_subdirectory(Animations)  # Nexium::World::Animation

# ======================= World =======================

add_library(Nexium_World_Base STATIC)
add_library(Nexium::World::Base ALIAS Nexium_World_Base)
target_sources(Nexium_World_Base PRIVATE
    Animations/FrameByFrame/FrameByFrameAnimator.cpp
    Assets/SkyboxAsset.cpp
    Entities/Actor.cpp
    Entities/Camera/Camera.cpp
    Framework/ActorManager.cpp
    Framework/CameraManager.cpp
    Framework/ControllerManager.cpp
    Framework/GameInstance.cpp
    Framework/InputController.cpp
    Framework/InputSystem.cpp
    Framework/GameInstanceSubsystem.cpp
    Framework/GameState.cpp
    Framework/LevelData.cpp
    Framework/PlayerState.cpp
    Framework/UserInterface.cpp
    Framework/World.cpp
    Framework/WorldObjectManager.cpp
    Scene/Grid.cpp
    Scene/Rectangle.cpp
    Scene/Scene.cpp
    Scene/SceneObject.cpp
    Scene/Spectator.cpp
)
nexium_add_public_headers(Nexium_World_Base
    NxWorld/Animations/FrameByFrame/FrameByFrameAnimator.h
    NxWorld/Animations/FrameByFrame/FrameByFrameAnimator.generated.h
    NxWorld/Assets/SkyboxAsset.h
    NxWorld/Assets/SkyboxAsset.generated.h
    NxWorld/Entities/Actor.h
    NxWorld/Entities/Actor.generated.h
    NxWorld/Entities/Camera/Camera.h
    NxWorld/Entities/Camera/Camera.generated.h
    NxWorld/Framework/ActorManager.h
    NxWorld/Framework/ApplicationIntegration.h
    NxWorld/Framework/CameraManager.h
    NxWorld/Framework/ControllerManager.h
    NxWorld/Framework/GameInstance.h
    NxWorld/Framework/InputController.h
    NxWorld/Framework/InputController.generated.h
    NxWorld/Framework/InputSystem.h
    NxWorld/Framework/GameInstance.generated.h
    NxWorld/Framework/GameInstanceSubsystem.h
    NxWorld/Framework/GameState.h
    NxWorld/Framework/LevelData.h
    NxWorld/Framework/PlayerState.h
    NxWorld/Framework/UserInterface.h
    NxWorld/Framework/World.h
    NxWorld/Framework/World.generated.h
    NxWorld/Framework/WorldObjectManager.h
    NxWorld/Scene/Grid.h
    NxWorld/Scene/Rectangle.h
    NxWorld/Scene/Rectangle.generated.h
    NxWorld/Scene/Scene.h
    NxWorld/Scene/Scene.generated.h
    NxWorld/Scene/SceneObject.h
    NxWorld/Scene/SceneObject.generated.h
    NxWorld/Scene/Spectator.h
    NxWorld/Scene/Spectator.generated.h
)
target_link_libraries(Nexium_World_Base
    PUBLIC
    Nexium::Foundation::Base
    Nexium::Foundation::Interfaces
    Nexium::Fundamental::Assets
    Nexium::Fundamental::Base
    Nexium::Fundamental::ECS
    Nexium::Fundamental::ResourceManagement
    JustReflectMe::Adapter
    Nexium::Platform::Input
    Nexium::RawBackend::Common
    Nexium::Subsystem::AssetsManager
    Nexium::Subsystem::Graphics
    Nexium::Subsystem::Input
    Utils::Core
    Nexium::World::Animation
    PRIVATE
    Nexium::Foundation::Debug
    Nexium::Fundamental::Utils
    glfw
    glm
    spdlog::spdlog
    ${MODULE_PRIVATE_TARGET}
)
target_precompile_headers(Nexium_World_Base PRIVATE WorldPCH.h)

if(NEXIUM_ENABLE_3D_MODULE)
    target_compile_definitions(Nexium_World_Base PRIVATE NEXIUM_ENABLE_3D_MODULE=1)
    target_sources(Nexium_World_Base PRIVATE
        Entities/Mesh/AssetImpls/StaticMeshBundle.cpp
        Entities/Mesh/StaticMesh.cpp
        Entities/Mesh/StaticMeshBundle.cpp
    )
    nexium_add_public_headers(Nexium_World_Base
        NxWorld/Entities/Mesh/AssetImpls/StaticMeshBundle.h
        NxWorld/Entities/Mesh/AssetImpls/StaticMeshBundle.generated.h
        NxWorld/Entities/Mesh/StaticMesh.h
        NxWorld/Entities/Mesh/StaticMesh.generated.h
        NxWorld/Entities/Mesh/StaticMeshBundle.h
        NxWorld/Entities/Mesh/StaticMeshBundle.generated.h
    )
    add_subdirectory(Entities/Mesh/AssimpMisc)  # Nexium::World::AssimpMisc
    target_link_libraries(Nexium_World_Base
        PUBLIC assimp::assimp
        PRIVATE
        Nexium::World::AssimpMisc
        Nexium::Subsystem::AssimpImporter
    )
endif()

foreach(TARGET_NAME IN ITEMS
    ${MODULE_PRIVATE_TARGET}
    Nexium_World_Base
)
    nexium_configure_target(${TARGET_NAME})
    add_dependencies(${TARGET_NAME} CodeGenerator)
endforeach()
