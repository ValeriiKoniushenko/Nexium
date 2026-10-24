add_library(Nexium_Fundamental_Assets STATIC)
add_library(Nexium::Fundamental::Assets ALIAS Nexium_Fundamental_Assets)
target_sources(Nexium_Fundamental_Assets PRIVATE
    BaseAsset.cpp
    ECSAsset.cpp
    Factory.cpp
)
nexium_add_public_headers(Nexium_Fundamental_Assets
    NxFundamental/Assets/BaseAsset.h
    NxFundamental/Assets/BaseAsset.generated.h
    NxFundamental/Assets/ECSAsset.h
    NxFundamental/Assets/ECSAsset.generated.h
    NxFundamental/Assets/Factory.h
)
target_link_libraries(Nexium_Fundamental_Assets
    PUBLIC
    Nexium::Foundation::Base
    Nexium::Fundamental::Base
    Nexium::Fundamental::ECS
    Nexium::Fundamental::ResourceManagement
    JustReflectMe::Adapter
    Utils::Core
    PRIVATE
    Nexium::Fundamental::Utils
    Utils::Utils
    Nexium_Fundamental_PrivateModule
)

nexium_configure_target(Nexium_Fundamental_Assets)
add_dependencies(Nexium_Fundamental_Assets CodeGenerator)
