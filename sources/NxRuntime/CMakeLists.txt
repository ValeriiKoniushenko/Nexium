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

# ====================== Runtime ======================

add_library(Nexium_Runtime STATIC)
add_library(Nexium::Runtime ALIAS Nexium_Runtime)
add_library(Nexium::NxRuntime ALIAS Nexium_Runtime)
target_sources(Nexium_Runtime PRIVATE
    GameUtils/FPSCounter.cpp
)
nexium_add_public_headers(Nexium_Runtime
    NxRuntime/GameUtils/FPSCounter.h
)
target_link_libraries(Nexium_Runtime PRIVATE ${MODULE_PRIVATE_TARGET})

target_link_libraries(Nexium_World PRIVATE Nexium_Runtime)

foreach(TARGET_NAME IN ITEMS
    ${MODULE_PRIVATE_TARGET}
    Nexium_Runtime
)
    nexium_configure_target(${TARGET_NAME})
endforeach()
