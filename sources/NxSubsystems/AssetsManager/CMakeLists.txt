add_library(Nexium_Subsystem_AssetsManager STATIC)
add_library(Nexium::Subsystem::AssetsManager ALIAS Nexium_Subsystem_AssetsManager)
target_sources(Nexium_Subsystem_AssetsManager PRIVATE
    AssetsManager.cpp
    TextureAsset.cpp
)
nexium_add_public_headers(Nexium_Subsystem_AssetsManager
    NxSubsystems/AssetsManager/AssetsManager.h
    NxSubsystems/AssetsManager/AssetsManager.generated.h
    NxSubsystems/AssetsManager/TextureAsset.h
    NxSubsystems/AssetsManager/TextureAsset.generated.h
)
target_link_libraries(Nexium_Subsystem_AssetsManager
    PUBLIC
    Nexium::Fundamental::Assets
    Nexium::Fundamental::ECS
    Nexium::Fundamental::ResourceManagement
    Nexium::Fundamental::TextureAtlas
    Nexium::RawBackend::Graphics
    Utils::Utils
    PRIVATE
    Nexium::Foundation::Base
    ${MODULE_PRIVATE_TARGET}
)
if(WIN32)
    target_link_libraries(Nexium_Subsystem_AssetsManager PRIVATE comdlg32)
endif()

nexium_configure_target(Nexium_Subsystem_AssetsManager)
add_dependencies(Nexium_Subsystem_AssetsManager CodeGenerator)
