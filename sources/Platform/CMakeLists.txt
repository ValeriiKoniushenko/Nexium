get_filename_component(MODULE_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
set(MODULE_PRIVATE_TARGET Nexium_${MODULE_NAME}_PrivateModule)

# ============== Per Module Private Info ===============

add_library(${MODULE_PRIVATE_TARGET} STATIC)
target_sources(${MODULE_PRIVATE_TARGET} PRIVATE
    PrivateModuleInfo.cpp
)
target_link_libraries(${MODULE_PRIVATE_TARGET} PRIVATE spdlog::spdlog)
target_compile_definitions(${MODULE_PRIVATE_TARGET} PRIVATE PRIVATE_MODULE_NAME="${MODULE_NAME}")

nexium_configure_target(${MODULE_PRIVATE_TARGET})

# ============== Sub-libraries ===============

add_subdirectory(Resources)  # Nexium::Platform::Resources
add_subdirectory(Graphics)   # Nexium::Platform::Graphics
add_subdirectory(Input)      # Nexium::Platform::Input
