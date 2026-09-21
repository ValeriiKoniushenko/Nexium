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

# ================ World::Animation =================

add_library(Nexium_World_Animation STATIC)
add_library(Nexium::World::Animation ALIAS Nexium_World_Animation)
target_sources(Nexium_World_Animation PRIVATE
    Animations/BaseAnimation.cpp
    Animations/FrameByFrame/FrameByFrameAnimation.cpp
)
nexium_add_public_headers(Nexium_World_Animation
    NxWorld/Animations/BaseAnimation.h
    NxWorld/Animations/BaseAnimation.generated.h
    NxWorld/Animations/FrameByFrame/FrameByFrameAnimation.h
    NxWorld/Animations/FrameByFrame/FrameByFrameAnimation.generated.h
)
target_link_libraries(Nexium_World_Animation
    PUBLIC Nexium::Fundamental::ECS
    PRIVATE ${MODULE_PRIVATE_TARGET}
)

# ======================= World =======================

add_library(Nexium_World STATIC)
add_library(Nexium::World ALIAS Nexium_World)
add_library(Nexium::NxWorld ALIAS Nexium_World)
add_library(Nexium::World::Scene ALIAS Nexium_World)
target_sources(Nexium_World PRIVATE
    Animations/FrameByFrame/FrameByFrameAnimator.cpp
    Assets/SkyboxAsset.cpp
    Entities/Actor.cpp
    Entities/Camera/Camera.cpp
    Framework/ActorManager.cpp
    Framework/CameraManager.cpp
    Framework/ControllerManager.cpp
    Framework/GameInstance.cpp
    Framework/GameInstanceSubsystem.cpp
    Framework/GameState.cpp
    Framework/LevelData.cpp
    Framework/PlayerState.cpp
    Framework/UserInterface.cpp
    Framework/World.cpp
    Framework/WorldObjectManager.cpp
    Scene/SceneObjects/Grid/Grid.cpp
    Scene/SceneObjects/Rectangle/Rectangle.cpp
    Scene/SceneObjects/Rectangle/RectangleAnimated.cpp
    Scene/Scene/Scene.cpp
    Scene/SceneObjects/SceneObject.cpp
    Scene/SceneObjects/Spectator/Spectator.cpp
    Scene/Scene/SceneManager.cpp
)
nexium_add_public_headers(Nexium_World
    NxWorld/Animations/FrameByFrame/FrameByFrameAnimator.h
    NxWorld/Animations/FrameByFrame/FrameByFrameAnimator.generated.h
    NxWorld/Assets/SkyboxAsset.h
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
    NxWorld/Framework/GameInstance.generated.h
    NxWorld/Framework/GameInstanceSubsystem.h
    NxWorld/Framework/GameState.h
    NxWorld/Framework/LevelData.h
    NxWorld/Framework/PlayerState.h
    NxWorld/Framework/UserInterface.h
    NxWorld/Framework/World.h
    NxWorld/Framework/World.generated.h
    NxWorld/Framework/WorldObjectManager.h
    NxWorld/Scene/SceneObjects/Grid/Grid.h
    NxWorld/Scene/SceneObjects/Rectangle/Rectangle.h
    NxWorld/Scene/SceneObjects/Rectangle/Rectangle.generated.h
    NxWorld/Scene/SceneObjects/Rectangle/RectangleAnimated.h
    NxWorld/Scene/SceneObjects/Rectangle/RectangleAnimated.generated.h
    NxWorld/Scene/Scene/Scene.h
    NxWorld/Scene/Scene/Scene.generated.h
    NxWorld/Scene/SceneObjects/SceneObject.h
    NxWorld/Scene/SceneObjects/SceneObject.generated.h
    NxWorld/Scene/SceneObjects/Spectator/Spectator.h
    NxWorld/Scene/Scene/SceneManager.h
    NxWorld/Scene/SceneObjects/Spectator/Spectator.generated.h
)
target_link_libraries(Nexium_World
    PUBLIC
        Nexium::World::Animation
        Nexium::Fundamental::Assets
        Nexium::Fundamental::ECS
        Nexium::Subsystem::AssetsManager
        Nexium::Subsystem::Graphics
        Nexium::Subsystem::Input
        Nexium::Platform::Graphics
        Nexium::Platform::Input
        Nexium::RawBackend
    PRIVATE
        ${MODULE_PRIVATE_TARGET}
        spdlog::spdlog
)

if(NEXIUM_ENABLE_3D_MODULE)
    target_compile_definitions(Nexium_World PRIVATE NEXIUM_ENABLE_3D_MODULE=1)
    target_sources(Nexium_World PRIVATE
        Entities/Mesh/AssetImpls/StaticMeshBundle.cpp
        Entities/Mesh/AssimpMisc/AssimpHelper.cpp
        Entities/Mesh/StaticMesh.cpp
        Entities/Mesh/StaticMeshBundle.cpp
    )
    nexium_add_public_headers(Nexium_World
        NxWorld/Entities/Mesh/AssetImpls/StaticMeshBundle.h
        NxWorld/Entities/Mesh/AssetImpls/StaticMeshBundle.generated.h
        NxWorld/Entities/Mesh/AssimpMisc/AssimpHelper.h
        NxWorld/Entities/Mesh/StaticMesh.h
        NxWorld/Entities/Mesh/StaticMesh.generated.h
        NxWorld/Entities/Mesh/StaticMeshBundle.h
        NxWorld/Entities/Mesh/StaticMeshBundle.generated.h
    )
    target_link_libraries(Nexium_World PUBLIC
        Nexium::Subsystem::AssimpImporter
        assimp::assimp
    )
endif()

foreach(TARGET_NAME IN ITEMS
    ${MODULE_PRIVATE_TARGET}
    Nexium_World_Animation
    Nexium_World
)
    nexium_configure_target(${TARGET_NAME})
    add_dependencies(${TARGET_NAME} CodeGenerator)
endforeach()
