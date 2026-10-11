get_filename_component(MODULE_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
set(MODULE_PRIVATE_TARGET Nexium_${MODULE_NAME}_PrivateModule)

# ============== Per Module Private Info ===============

add_library(${MODULE_PRIVATE_TARGET} STATIC)
target_sources(${MODULE_PRIVATE_TARGET} PRIVATE
    PrivateModuleInfo.cpp
)
target_link_libraries(${MODULE_PRIVATE_TARGET} PRIVATE spdlog::spdlog)
target_compile_definitions(${MODULE_PRIVATE_TARGET} PRIVATE PRIVATE_MODULE_NAME="${MODULE_NAME}")

# ================ Graphics =================
add_library(Nexium_RawBackend_Graphics STATIC)
add_library(Nexium::RawBackend::Graphics ALIAS Nexium_RawBackend_Graphics)
target_sources(Nexium_RawBackend_Graphics PRIVATE
    Graphics/Texture.cpp
    Graphics/RenderTargetToTexture.cpp
)
nexium_add_public_headers(Nexium_RawBackend_Graphics
    RawBackend/Graphics/Texture.h
    RawBackend/Graphics/RenderTargetToTexture.h
)

target_link_libraries(Nexium_RawBackend_Graphics
    PUBLIC
    Nexium::Foundation
    Nexium::Platform::Graphics
    PRIVATE
    Core::Stb
    ${MODULE_PRIVATE_TARGET}
)

# ================ Fonts =================
add_library(Nexium_RawBackend_Fonts STATIC)
add_library(Nexium::RawBackend::Fonts ALIAS Nexium_RawBackend_Fonts)
target_sources(Nexium_RawBackend_Fonts PRIVATE
    Fonts/FreeTypeLibrary.cpp
)
nexium_add_public_headers(Nexium_RawBackend_Fonts
    RawBackend/Fonts/FreeTypeLibrary.h
)

target_link_libraries(Nexium_RawBackend_Fonts
    PUBLIC
    Nexium::Foundation
    PRIVATE
    freetype
    Core::Stb
    ${MODULE_PRIVATE_TARGET}
)

# ================ RawBackend =================
add_library(Nexium_RawBackend STATIC)
add_library(Nexium::RawBackend ALIAS Nexium_RawBackend)
target_sources(Nexium_RawBackend PRIVATE
    IDrawable.cpp
    Image.cpp
    Utils.cpp
)
nexium_add_public_headers(Nexium_RawBackend
    RawBackend/IDrawable.h
    RawBackend/Image.h
    RawBackend/Utils.h
)

target_link_libraries(Nexium_RawBackend
    PUBLIC
    Nexium::RawBackend::Graphics
    Nexium::RawBackend::Fonts
    PRIVATE
    Core::Stb
    ${MODULE_PRIVATE_TARGET}
)

# =========================================

nexium_configure_target(${MODULE_PRIVATE_TARGET})
nexium_configure_target(Nexium_RawBackend)

add_dependencies(Nexium_RawBackend CodeGenerator)
add_dependencies(Nexium_RawBackend_Fonts CodeGenerator)
add_dependencies(Nexium_RawBackend_Graphics CodeGenerator)
