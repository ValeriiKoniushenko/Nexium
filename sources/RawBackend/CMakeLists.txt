get_filename_component(MODULE_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
set(MODULE_PRIVATE_TARGET Nexium_${MODULE_NAME}_PrivateModule)

# ============== Per Module Private Info ===============

add_library(${MODULE_PRIVATE_TARGET} STATIC)
target_sources(${MODULE_PRIVATE_TARGET} PRIVATE
    PrivateModuleInfo.cpp
)
target_link_libraries(${MODULE_PRIVATE_TARGET} PRIVATE spdlog::spdlog)
target_compile_definitions(${MODULE_PRIVATE_TARGET} PRIVATE PRIVATE_MODULE_NAME="${MODULE_NAME}")

# ============== Sub-libraries ===============

add_subdirectory(Graphics)
add_subdirectory(Fonts)

# ================ RawBackend =================
add_library(Nexium_RawBackend_Common STATIC)
add_library(Nexium::RawBackend::Common ALIAS Nexium_RawBackend_Common)
target_sources(Nexium_RawBackend_Common PRIVATE
    IDrawable.cpp
    Image.cpp
    Utils.cpp
)
nexium_add_public_headers(Nexium_RawBackend_Common
    RawBackend/IDrawable.h
    RawBackend/Image.h
    RawBackend/Utils.h
)

target_link_libraries(Nexium_RawBackend_Common
    PUBLIC
    Nexium::Foundation
    JustReflectMe::Adapter
    Nexium::Platform::Graphics
    Utils::Core
    PRIVATE
    Core::Stb
    ${MODULE_PRIVATE_TARGET}
)

# =========================================

nexium_configure_target(${MODULE_PRIVATE_TARGET})
nexium_configure_target(Nexium_RawBackend_Common)

add_dependencies(Nexium_RawBackend_Common CodeGenerator)
