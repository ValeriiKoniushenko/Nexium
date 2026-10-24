add_library(Nexium_RawBackend_Graphics STATIC)
add_library(Nexium::RawBackend::Graphics ALIAS Nexium_RawBackend_Graphics)
target_sources(Nexium_RawBackend_Graphics PRIVATE
    Texture.cpp
    RenderTargetToTexture.cpp
)
nexium_add_public_headers(Nexium_RawBackend_Graphics
    RawBackend/Graphics/Texture.h
    RawBackend/Graphics/RenderTargetToTexture.h
)

target_link_libraries(Nexium_RawBackend_Graphics
    PUBLIC
    Nexium::Platform::Graphics
    Nexium::RawBackend::Common
    Utils::Core
    PRIVATE
    Nexium::Foundation::Base
    ${MODULE_PRIVATE_TARGET}
)

nexium_configure_target(Nexium_RawBackend_Graphics)
add_dependencies(Nexium_RawBackend_Graphics CodeGenerator)
