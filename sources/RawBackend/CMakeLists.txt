# ============== Per Module Private Info ===============

add_library(Nexium_RawBackend_PrivateModule STATIC)
target_sources(Nexium_RawBackend_PrivateModule PRIVATE
    PrivateModuleInfo.cpp
    PrivateModuleInfo.h
)
target_include_directories(Nexium_RawBackend_PrivateModule PRIVATE ../)
target_link_libraries(Nexium_RawBackend_PrivateModule PRIVATE Nexium::Foundation)
target_compile_definitions(Nexium_RawBackend_PrivateModule PRIVATE PRIVATE_MODULE_NAME="RawBackend")

CoreAddCompileOptionsTo(Nexium_RawBackend_PrivateModule)

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
set_source_files_properties(${SOURCES} PROPERTIES LANGUAGE CXX)
target_sources(Nexium_RawBackend PRIVATE ${SOURCES} PUBLIC ${HEADERS})

target_include_directories(Nexium_RawBackend PUBLIC ../)
target_link_libraries(Nexium_RawBackend
    PUBLIC
    Nexium::Foundation
    Core::Stb
    PRIVATE
    JustReflectMe::Adapter
    Nexium_RawBackend_PrivateModule

)

# =========================================

CoreAddCompileOptionsTo(Nexium_RawBackend_PrivateModule)
CoreAddCompileOptionsTo(Nexium_RawBackend)

add_dependencies(Nexium_RawBackend_PrivateModule CodeGenerator)
add_dependencies(Nexium_RawBackend CodeGenerator)
