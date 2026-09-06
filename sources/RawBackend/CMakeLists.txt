# ============== Per Module Private Info ===============

add_library(Nexium_RawBackend_PrivateModule STATIC)
target_sources(Nexium_RawBackend_PrivateModule PRIVATE
    PrivateModuleInfo.cpp
    PrivateModuleInfo.h
)
target_link_libraries(Nexium_RawBackend_PrivateModule PRIVATE Nexium::Foundation)
target_compile_definitions(Nexium_RawBackend_PrivateModule PRIVATE PRIVATE_MODULE_NAME="RawBackend")

# ================ RawBackend =================
set(SOURCES
    IDrawable.cpp
    Image.cpp
)

set(HEADERS
    IDrawable.h
    Image.h
)

add_library(Nexium_RawBackend STATIC)
add_library(Nexium::RawBackend ALIAS Nexium_RawBackend)
target_sources(Nexium_RawBackend PRIVATE ${SOURCES} ${HEADERS})

target_link_libraries(Nexium_RawBackend
    PUBLIC
    Nexium::Foundation
    JustReflectMe::Adapter
    PRIVATE
    Core::Stb
    Nexium_RawBackend_PrivateModule
)

# =========================================

nexium_configure_target(Nexium_RawBackend_PrivateModule)
nexium_configure_target(Nexium_RawBackend)

add_dependencies(Nexium_RawBackend CodeGenerator)
