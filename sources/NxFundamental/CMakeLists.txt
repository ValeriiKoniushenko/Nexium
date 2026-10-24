# ============== Per Module Private Info ===============

add_library(Nexium_Fundamental_PrivateModule STATIC)
target_sources(Nexium_Fundamental_PrivateModule PRIVATE
    PrivateModuleInfo.cpp
)
target_link_libraries(Nexium_Fundamental_PrivateModule PRIVATE spdlog::spdlog)
target_compile_definitions(Nexium_Fundamental_PrivateModule
    PRIVATE PRIVATE_MODULE_NAME="NxFundamental"
)

nexium_configure_target(Nexium_Fundamental_PrivateModule)
add_dependencies(Nexium_Fundamental_PrivateModule CodeGenerator)

# ================ Sub-libraries =================

add_subdirectory(Utils)               # Nexium::Fundamental::Utils

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
)
target_link_libraries(Nexium_Fundamental_Base
    PUBLIC
    Nexium::Fundamental::ResourceManagement
    Utils::Core
    PRIVATE
    Nexium::Fundamental::Utils
    glm
)

nexium_configure_target(Nexium_Fundamental_Base)
add_dependencies(Nexium_Fundamental_Base CodeGenerator)

add_subdirectory(ResourceManagement)  # Nexium::Fundamental::ResourceManagement
add_subdirectory(ECS)                 # Nexium::Fundamental::ECS
add_subdirectory(Assets)              # Nexium::Fundamental::Assets
add_subdirectory(TextureAtlas)        # Nexium::Fundamental::TextureAtlas
