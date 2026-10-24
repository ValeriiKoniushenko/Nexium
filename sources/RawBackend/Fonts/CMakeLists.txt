add_library(Nexium_RawBackend_Fonts STATIC)
add_library(Nexium::RawBackend::Fonts ALIAS Nexium_RawBackend_Fonts)
target_sources(Nexium_RawBackend_Fonts PRIVATE
    FreeTypeLibrary.cpp
)
nexium_add_public_headers(Nexium_RawBackend_Fonts
    RawBackend/Fonts/FreeTypeLibrary.h
)

target_link_libraries(Nexium_RawBackend_Fonts
    PUBLIC
    Nexium::Foundation::Base
    Utils::Core
    freetype
    PRIVATE
    ${MODULE_PRIVATE_TARGET}
)

nexium_configure_target(Nexium_RawBackend_Fonts)
add_dependencies(Nexium_RawBackend_Fonts CodeGenerator)
