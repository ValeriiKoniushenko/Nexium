add_library(Nexium_World_Animation STATIC)
add_library(Nexium::World::Animation ALIAS Nexium_World_Animation)
target_sources(Nexium_World_Animation PRIVATE
    BaseAnimation.cpp
    FrameByFrame/FrameByFrameAnimation.cpp
)
nexium_add_public_headers(Nexium_World_Animation
    NxWorld/Animations/BaseAnimation.h
    NxWorld/Animations/BaseAnimation.generated.h
    NxWorld/Animations/FrameByFrame/FrameByFrameAnimation.h
    NxWorld/Animations/FrameByFrame/FrameByFrameAnimation.generated.h
)
target_link_libraries(Nexium_World_Animation
    PUBLIC
    Nexium::Foundation::Base
    Nexium::Fundamental::ECS
    Nexium::Fundamental::ResourceManagement
    JustReflectMe::Adapter
    Utils::Core
    PRIVATE
    ${MODULE_PRIVATE_TARGET}
)

nexium_configure_target(Nexium_World_Animation)
add_dependencies(Nexium_World_Animation CodeGenerator)
