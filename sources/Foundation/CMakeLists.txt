get_filename_component(MODULE_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
set(MODULE_PRIVATE_TARGET Nexium_${MODULE_NAME}_PrivateModule)

# ============== Per Module Private Info ===============

if(0) # It's disabled due to it's unused. Feel free to active it when it will be needed.
    add_library(${MODULE_PRIVATE_TARGET} STATIC)
    target_sources(${MODULE_PRIVATE_TARGET} PRIVATE
        PrivateModuleInfo.cpp
    )
    target_link_libraries(${MODULE_PRIVATE_TARGET} PRIVATE spdlog::spdlog)
    target_compile_definitions(${MODULE_PRIVATE_TARGET} PRIVATE PRIVATE_MODULE_NAME="${MODULE_NAME}")
    nexium_configure_target(${MODULE_PRIVATE_TARGET})
endif()

# ============== Foundation ===============
add_subdirectory(Debug)       # Nexium::Foundation::Debug

add_library(Nexium_Foundation_Base STATIC)
add_library(Nexium::Foundation::Base ALIAS Nexium_Foundation_Base)
target_sources(Nexium_Foundation_Base PRIVATE
    Configs.cpp
    BaseLog.cpp
)

nexium_add_public_headers(Nexium_Foundation_Base
    Foundation/Configs.h
    Foundation/BaseLog.h
    Foundation/BaseUsings.h
)

target_link_libraries(Nexium_Foundation_Base
    PUBLIC
    Utils::Core
    spdlog::spdlog
)

if(TARGET ${MODULE_PRIVATE_TARGET})
    target_link_libraries(Nexium_Foundation_Base PRIVATE ${MODULE_PRIVATE_TARGET})
endif()

# =========================================

nexium_configure_target(Nexium_Foundation_Base PROJECT_DIR)

add_dependencies(Nexium_Foundation_Base CodeGenerator)

# Interfaces depends on Foundation, so it is added after the Foundation target exists.
add_subdirectory(Interfaces)  # Nexium::Foundation::Interfaces
