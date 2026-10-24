add_library(Nexium_Fundamental_TextureAtlas STATIC)
add_library(Nexium::Fundamental::TextureAtlas ALIAS Nexium_Fundamental_TextureAtlas)
target_sources(Nexium_Fundamental_TextureAtlas PRIVATE
    TextureAtlas.cpp
)
nexium_add_public_headers(Nexium_Fundamental_TextureAtlas
    NxFundamental/TextureAtlas/TextureAtlas.h
)
target_link_libraries(Nexium_Fundamental_TextureAtlas
    PUBLIC
    Nexium::Foundation
    Nexium::RawBackend::Common
    Nexium::RawBackend::Graphics
    Utils::Core
    PRIVATE
    Core::Rectpack2D
    Nexium_Fundamental_PrivateModule
)

nexium_configure_target(Nexium_Fundamental_TextureAtlas)
add_dependencies(Nexium_Fundamental_TextureAtlas CodeGenerator)
