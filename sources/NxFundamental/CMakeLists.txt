# ============== Per Module Private Info ===============

add_library(Nexium_Fundamental_PrivateModule STATIC)
target_sources(Nexium_Fundamental_PrivateModule PRIVATE
    PrivateModuleInfo.cpp
)
target_link_libraries(Nexium_Fundamental_PrivateModule PRIVATE spdlog::spdlog)
target_compile_definitions(Nexium_Fundamental_PrivateModule
    PRIVATE PRIVATE_MODULE_NAME="NxFundamental"
)

# ================ Fundamental base =================

add_library(Nexium_Fundamental_Base STATIC)
add_library(Nexium::Fundamental::Base ALIAS Nexium_Fundamental_Base)
target_sources(Nexium_Fundamental_Base PRIVATE
    ITagHolder.cpp
    Transformable.cpp
)
nexium_add_public_headers(Nexium_Fundamental_Base
    NxFundamental/ITagHolder.h
    NxFundamental/Transformable.h
    NxFundamental/Transformable.generated.h
)
target_link_libraries(Nexium_Fundamental_Base PUBLIC Nexium::Foundation)

# ================ Resource management =================

add_library(Nexium_Fundamental_ResourceManagement STATIC)
add_library(Nexium::Fundamental::ResourceManagement
    ALIAS Nexium_Fundamental_ResourceManagement
)
target_sources(Nexium_Fundamental_ResourceManagement PRIVATE
    ResourceManagement/DataStream.cpp
    ResourceManagement/JsonAdapter.cpp
)
nexium_add_public_headers(Nexium_Fundamental_ResourceManagement
    NxFundamental/ResourceManagement/DataStream.h
    NxFundamental/ResourceManagement/DataStream.generated.h
    NxFundamental/ResourceManagement/JsonAdapter.h
)
target_link_libraries(Nexium_Fundamental_ResourceManagement
    PUBLIC Nexium::Foundation
    PRIVATE Nexium_Fundamental_PrivateModule
)

# ================ ECS =================

add_library(Nexium_Fundamental_ECS STATIC)
add_library(Nexium::Fundamental::ECS ALIAS Nexium_Fundamental_ECS)
target_sources(Nexium_Fundamental_ECS PRIVATE
    ECS/BaseComponent.cpp
)
nexium_add_public_headers(Nexium_Fundamental_ECS
    NxFundamental/ECS/BaseComponent.h
    NxFundamental/ECS/BaseComponent.generated.h
)
target_link_libraries(Nexium_Fundamental_ECS
    PUBLIC
        Nexium::Fundamental::Base
        Nexium::Fundamental::ResourceManagement
    PRIVATE Nexium_Fundamental_PrivateModule
)

# ================ Assets =================

add_library(Nexium_Fundamental_Assets STATIC)
add_library(Nexium::Fundamental::Assets ALIAS Nexium_Fundamental_Assets)
target_sources(Nexium_Fundamental_Assets PRIVATE
    Assets/BaseAsset.cpp
    Assets/ECSAsset.cpp
    Assets/Factory.cpp
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
        Nexium::Fundamental::ECS
        Nexium::Fundamental::ResourceManagement
        Nexium::Platform::Graphics
        Core::Rectpack2D
    PRIVATE Nexium_Fundamental_PrivateModule
)

# ================ Texture atlas =================

add_library(Nexium_Fundamental_TextureAtlas STATIC)
add_library(Nexium::Fundamental::TextureAtlas ALIAS Nexium_Fundamental_TextureAtlas)
target_sources(Nexium_Fundamental_TextureAtlas PRIVATE
    Assets/TextureAtlas.cpp
)
nexium_add_public_headers(Nexium_Fundamental_TextureAtlas
    NxFundamental/Assets/TextureAtlas.h
)
target_link_libraries(Nexium_Fundamental_TextureAtlas
    PUBLIC
        Nexium::Foundation
        Nexium::RawBackend
        Core::Rectpack2D
    PRIVATE Nexium_Fundamental_PrivateModule
)

# ================ Aggregate compatibility target =================

add_library(Nexium_Fundamental INTERFACE)
add_library(Nexium::Fundamental ALIAS Nexium_Fundamental)
# The old target name was derived from the directory name. Keep it usable while
# consumers move to the narrower public targets above.
add_library(Nexium::NxFundamental ALIAS Nexium_Fundamental)
target_link_libraries(Nexium_Fundamental INTERFACE
    Nexium::Fundamental::Base
    Nexium::Fundamental::ResourceManagement
    Nexium::Fundamental::ECS
    Nexium::Fundamental::Assets
    Nexium::Fundamental::TextureAtlas
)

# =========================================

foreach(TARGET_NAME IN ITEMS
    Nexium_Fundamental_PrivateModule
    Nexium_Fundamental_Base
    Nexium_Fundamental_ResourceManagement
    Nexium_Fundamental_ECS
    Nexium_Fundamental_Assets
    Nexium_Fundamental_TextureAtlas
)
    nexium_configure_target(${TARGET_NAME})
    add_dependencies(${TARGET_NAME} CodeGenerator)
endforeach()

nexium_enable_debug_api(Nexium_Fundamental_ECS)
