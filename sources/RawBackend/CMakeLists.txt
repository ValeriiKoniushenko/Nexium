get_filename_component(MODULE_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
set(MODULE_PRIVATE_TARGET Nexium_${MODULE_NAME}_PrivateModule)

# ============== Per Module Private Info ===============

add_library(${MODULE_PRIVATE_TARGET} STATIC)
target_sources(${MODULE_PRIVATE_TARGET} PRIVATE
    PrivateModuleInfo.cpp
)
target_link_libraries(${MODULE_PRIVATE_TARGET} PRIVATE spdlog::spdlog)
target_compile_definitions(${MODULE_PRIVATE_TARGET} PRIVATE PRIVATE_MODULE_NAME="${MODULE_NAME}")

# ================ RawBackend =================
add_library(Nexium_RawBackend STATIC)
add_library(Nexium::RawBackend ALIAS Nexium_RawBackend)
target_sources(Nexium_RawBackend PRIVATE
    Graphics/Texture.cpp
    Graphics/RenderTargetToTexture.cpp
    IDrawable.cpp
    Image.cpp
    Utils.cpp
)
nexium_add_public_headers(Nexium_RawBackend
    RawBackend/Graphics/Texture.h
    RawBackend/Graphics/RenderTargetToTexture.h
    RawBackend/IDrawable.h
    RawBackend/IDrawable.generated.h
    RawBackend/Image.h
    RawBackend/Image.generated.h
    RawBackend/Utils.h
)

target_link_libraries(Nexium_RawBackend
    PUBLIC
    Nexium::Foundation
    Nexium::Platform::Graphics
    PRIVATE
    Core::Stb
    ${MODULE_PRIVATE_TARGET}
)

# =========================================

nexium_configure_target(${MODULE_PRIVATE_TARGET})
nexium_configure_target(Nexium_RawBackend)

add_dependencies(Nexium_RawBackend CodeGenerator)
