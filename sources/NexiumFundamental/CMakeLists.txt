# ============== Per Module Private Info ===============

get_filename_component(MODULE_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
set(MODULE_TARGET Nexium_${MODULE_NAME})
set(MODULE_PRIVATE_TARGET ${MODULE_TARGET}_PrivateModule)
set(MODULE_ALIAS Nexium::${MODULE_NAME})

add_library(${MODULE_PRIVATE_TARGET} STATIC)
target_sources(${MODULE_PRIVATE_TARGET} PRIVATE
    PrivateModuleInfo.cpp
    PrivateModuleInfo.h
)
target_link_libraries(${MODULE_PRIVATE_TARGET} PRIVATE Nexium::Foundation)
target_compile_definitions(${MODULE_PRIVATE_TARGET} PRIVATE PRIVATE_MODULE_NAME="${MODULE_NAME}")

# ================ NexiumFundamental =================
set(SOURCES
    Assets/BaseAsset.cpp
    Assets/ECSAsset.cpp
    Assets/TextureAtlas.cpp
    
    ECS/BaseComponent.cpp

    ResourceManagement/DataStream.cpp
    ResourceManagement/JsonAdapter.cpp

    ITagHolder.cpp
    Transformable.cpp
)

set(HEADERS
    Assets/BaseAsset.h
    Assets/ECSAsset.h
    Assets/TextureAtlas.h

    ECS/BaseComponent.h

    ResourceManagement/DataStream.h
    ResourceManagement/JsonAdapter.h

    ITagHolder.h
    Transformable.h
)

add_library(${MODULE_TARGET} STATIC)
add_library(${MODULE_ALIAS} ALIAS ${MODULE_TARGET})
target_sources(${MODULE_TARGET} PRIVATE ${SOURCES} ${HEADERS})

target_link_libraries(${MODULE_TARGET}
    PUBLIC
    Nexium::RawBackend
    JustReflectMe::Adapter
    PRIVATE
    Core::Rectpack2D
    ${MODULE_PRIVATE_TARGET}
)

# =========================================

nexium_configure_target(${MODULE_PRIVATE_TARGET})
nexium_configure_target(${MODULE_TARGET})

add_dependencies(${MODULE_TARGET} CodeGenerator)
