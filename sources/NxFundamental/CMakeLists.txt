# ============== Per Module Private Info ===============

add_library(Nexium_Fundamental_PrivateModule STATIC)
target_sources(Nexium_Fundamental_PrivateModule PRIVATE
    PrivateModuleInfo.cpp
    PrivateModuleInfo.h
)
target_link_libraries(Nexium_Fundamental_PrivateModule PRIVATE Nexium::Foundation)
target_compile_definitions(Nexium_Fundamental_PrivateModule
    PRIVATE PRIVATE_MODULE_NAME="NxFundamental"
)

# ================ Fundamental base =================

add_library(Nexium_Fundamental_Base STATIC)
add_library(Nexium::Fundamental::Base ALIAS Nexium_Fundamental_Base)
target_sources(Nexium_Fundamental_Base PRIVATE
    ITagHolder.cpp
    ITagHolder.h
    Transformable.cpp
    Transformable.h
)
target_link_libraries(Nexium_Fundamental_Base PUBLIC Nexium::Foundation)

# ================ Resource management =================

add_library(Nexium_Fundamental_ResourceManagement STATIC)
add_library(Nexium::Fundamental::ResourceManagement
    ALIAS Nexium_Fundamental_ResourceManagement
)
target_sources(Nexium_Fundamental_ResourceManagement PRIVATE
    ResourceManagement/DataStream.cpp
    ResourceManagement/DataStream.h
    ResourceManagement/JsonAdapter.cpp
    ResourceManagement/JsonAdapter.h
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
    ECS/BaseComponent.h
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
    Assets/BaseAsset.h
    Assets/ECSAsset.cpp
    Assets/ECSAsset.h
    Assets/Factory.h
    Assets/Factory.cpp
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
    Assets/TextureAtlas.h
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
