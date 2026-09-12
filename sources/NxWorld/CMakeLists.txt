get_filename_component(MODULE_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
set(MODULE_TARGET Nexium_${MODULE_NAME})
set(MODULE_PRIVATE_TARGET ${MODULE_TARGET}_PrivateModule)
set(MODULE_ALIAS Nexium::${MODULE_NAME})

# ============== Per Module Private Info ===============

add_library(${MODULE_PRIVATE_TARGET} STATIC)
target_sources(${MODULE_PRIVATE_TARGET} PRIVATE
    PrivateModuleInfo.cpp
)
target_link_libraries(${MODULE_PRIVATE_TARGET} PRIVATE spdlog::spdlog)
target_compile_definitions(${MODULE_PRIVATE_TARGET} PRIVATE PRIVATE_MODULE_NAME="${MODULE_NAME}")

nexium_configure_target(${MODULE_PRIVATE_TARGET})

# ================ Animation =================

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
    PRIVATE Nexium_NxSubsystems_PrivateModule
)
nexium_configure_target(Nexium_World_Animation)
add_dependencies(Nexium_World_Animation CodeGenerator)
