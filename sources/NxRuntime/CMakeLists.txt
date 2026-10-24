get_filename_component(MODULE_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
set(MODULE_PRIVATE_TARGET Nexium_${MODULE_NAME}_PrivateModule)

# ============== Per Module Private Info ===============

add_library(${MODULE_PRIVATE_TARGET} STATIC)
target_sources(${MODULE_PRIVATE_TARGET} PRIVATE
    PrivateModuleInfo.cpp
    PrivateModuleInfo.h
)
target_link_libraries(${MODULE_PRIVATE_TARGET} PRIVATE spdlog::spdlog)
target_compile_definitions(${MODULE_PRIVATE_TARGET} PRIVATE PRIVATE_MODULE_NAME="${MODULE_NAME}")

# ============== Sub-libraries ===============

add_subdirectory(GameUtils)  # Nexium::Runtime::GameUtils

# ====================== Runtime ======================

add_library(Nexium_Runtime_Base STATIC)
add_library(Nexium::Runtime::Base ALIAS Nexium_Runtime_Base)
target_sources(Nexium_Runtime_Base PRIVATE
    Runtime.cpp
)
nexium_add_public_headers(Nexium_Runtime_Base
    NxRuntime/Runtime.h
)
target_link_libraries(Nexium_Runtime_Base
    PUBLIC
    "$<LINK_LIBRARY:WHOLE_ARCHIVE,Nexium::World::Base>"
    PRIVATE
    Nexium::Foundation::Debug
    Nexium::Runtime::GameUtils
    Utils::Core
    ${MODULE_PRIVATE_TARGET}
)

foreach(TARGET_NAME IN ITEMS
    ${MODULE_PRIVATE_TARGET}
    Nexium_Runtime_Base
)
    nexium_configure_target(${TARGET_NAME})
endforeach()
