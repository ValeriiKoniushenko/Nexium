add_library(Nexium_Fundamental_ECS STATIC)
add_library(Nexium::Fundamental::ECS ALIAS Nexium_Fundamental_ECS)
target_sources(Nexium_Fundamental_ECS PRIVATE
    BaseComponent.cpp
)
nexium_add_public_headers(Nexium_Fundamental_ECS
    NxFundamental/ECS/BaseComponent.h
)
target_link_libraries(Nexium_Fundamental_ECS
    PUBLIC
    Nexium::Foundation
    Nexium::Fundamental::Base
    Nexium::Fundamental::ResourceManagement
    Utils::Core
    PRIVATE
    Nexium_Fundamental_PrivateModule
)

nexium_configure_target(Nexium_Fundamental_ECS)
add_dependencies(Nexium_Fundamental_ECS CodeGenerator)
nexium_enable_debug_api(Nexium_Fundamental_ECS)
