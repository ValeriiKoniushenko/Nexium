get_filename_component(MODULE_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
set(MODULE_TARGET Nexium_${MODULE_NAME})
set(MODULE_PRIVATE_TARGET ${MODULE_TARGET}_PrivateModule)
set(MODULE_ALIAS Nexium::${MODULE_NAME})

# ============== Per Module Private Info ===============

add_library(${MODULE_PRIVATE_TARGET} STATIC)
target_sources(${MODULE_PRIVATE_TARGET} PRIVATE
    PrivateModuleInfo.cpp
    PrivateModuleInfo.h
)
target_link_libraries(${MODULE_PRIVATE_TARGET} PRIVATE spdlog::spdlog Utils::Core)
target_compile_definitions(${MODULE_PRIVATE_TARGET} PRIVATE PRIVATE_MODULE_NAME="${MODULE_NAME}")

# ================ RawBackend =================
add_library(${MODULE_TARGET} STATIC)
add_library(${MODULE_ALIAS} ALIAS ${MODULE_TARGET})
target_sources(${MODULE_TARGET} PRIVATE
    Graphics/Texture.h
    Graphics/Texture.cpp
    Graphics/RenderTargetToTexture.h
    Graphics/RenderTargetToTexture.cpp

    IDrawable.h
    IDrawable.cpp
    Image.h
    Image.cpp
)

target_link_libraries(${MODULE_TARGET}
    PUBLIC
    Nexium::Foundation
    Nexium::Platform::Graphics
    PRIVATE
    Core::Stb
    ${MODULE_PRIVATE_TARGET}
)

# =========================================

nexium_configure_target(${MODULE_PRIVATE_TARGET})
nexium_configure_target(${MODULE_TARGET})

add_dependencies(${MODULE_TARGET} CodeGenerator)
