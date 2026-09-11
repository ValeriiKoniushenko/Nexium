get_filename_component(MODULE_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
set(MODULE_PRIVATE_TARGET Nexium_${MODULE_NAME}_PrivateModule)

# ============== Per Module Private Info ===============

if(0) # It's disabled due to it's unused. Feel free to active it when it will be needed.
    add_library(${MODULE_PRIVATE_TARGET} STATIC)
    target_sources(${MODULE_PRIVATE_TARGET} PRIVATE
        PrivateModuleInfo.cpp
    )
    target_link_libraries(${MODULE_PRIVATE_TARGET} PRIVATE spdlog::spdlog Utils::Core)
    target_compile_definitions(${MODULE_PRIVATE_TARGET} PRIVATE PRIVATE_MODULE_NAME="${MODULE_NAME}")
    nexium_configure_target(${MODULE_PRIVATE_TARGET})
endif()

# ============== Foundation ===============
add_library(Nexium_Foundation STATIC)
add_library(Nexium::Foundation ALIAS Nexium_Foundation)
target_sources(Nexium_Foundation PRIVATE
    Configs.cpp
    BaseLog.cpp
    Interfaces/DataStream.cpp
)

nexium_add_public_headers(Nexium_Foundation
    Foundation/Configs.h
    Foundation/BaseLog.h
    Foundation/Interfaces/DataStream.h
    Foundation/Interfaces/IOutliner.h
    Foundation/Interfaces/IOutliner.generated.h
)

target_link_libraries(Nexium_Foundation
    PUBLIC
    Utils::Core
    spdlog::spdlog
    JustReflectMe::Adapter
)

if(TARGET ${MODULE_PRIVATE_TARGET})
    target_link_libraries(Nexium_Foundation PRIVATE ${MODULE_PRIVATE_TARGET})
endif()

# =========================================

nexium_configure_target(Nexium_Foundation PROJECT_DIR)

add_dependencies(Nexium_Foundation CodeGenerator)
