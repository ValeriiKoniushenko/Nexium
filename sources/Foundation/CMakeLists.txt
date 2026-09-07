# ============== Foundation ===============

set(SOURCES
    Configs.cpp
    BaseLog.cpp

    Interfaces/DataStream.cpp
)

set(HEADERS
    Configs.h
    BaseLog.h

    Interfaces/DataStream.h
)

add_library(Nexium_Foundation STATIC)
add_library(Nexium::Foundation ALIAS Nexium_Foundation)
target_sources(Nexium_Foundation PRIVATE ${SOURCES} ${HEADERS})

target_link_libraries(Nexium_Foundation PUBLIC
    Utils::Core
    Utils::Utils
    spdlog::spdlog
)

# ============== Per Module Private Info ===============

add_library(Nexium_Foundation_PrivateModule STATIC)
target_sources(Nexium_Foundation_PrivateModule PRIVATE
    PrivateModuleInfo.cpp
    PrivateModuleInfo.h
)
target_link_libraries(Nexium_Foundation_PrivateModule PRIVATE Nexium::Foundation)
get_filename_component(CURRENT_DIR_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
target_compile_definitions(Nexium_Foundation_PrivateModule PRIVATE PRIVATE_MODULE_NAME="${CURRENT_DIR_NAME}")

# =========================================

nexium_configure_target(Nexium_Foundation_PrivateModule)
nexium_configure_target(Nexium_Foundation)

add_dependencies(Nexium_Foundation CodeGenerator)
