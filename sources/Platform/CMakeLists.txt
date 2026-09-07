get_filename_component(MODULE_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
set(MODULE_TARGET_PREFIX Nexium_${MODULE_NAME})
set(MODULE_PRIVATE_TARGET ${MODULE_TARGET_PREFIX}_PrivateModule)
set(MODULE_ALIAS_PREFIX Nexium::${MODULE_NAME})
set(GRAPHICS_TARGET ${MODULE_TARGET_PREFIX}_Graphics)
set(GRAPHICS_ALIAS ${MODULE_ALIAS_PREFIX}::Graphics)
set(INPUT_TARGET ${MODULE_TARGET_PREFIX}_Input)
set(INPUT_ALIAS ${MODULE_ALIAS_PREFIX}::Input)

# ============== Per Module Private Info ===============

add_library(${MODULE_PRIVATE_TARGET} STATIC)
target_sources(${MODULE_PRIVATE_TARGET} PRIVATE
    PrivateModuleInfo.cpp
    PrivateModuleInfo.h
)
target_link_libraries(${MODULE_PRIVATE_TARGET} PRIVATE Nexium::Foundation)
target_compile_definitions(${MODULE_PRIVATE_TARGET} PRIVATE PRIVATE_MODULE_NAME="${MODULE_NAME}")

# ============== Graphics ===============

set(SOURCES
    Graphics.cpp
)

set(HEADERS
    Graphics.h
)

add_library(${GRAPHICS_TARGET} STATIC)
add_library(${GRAPHICS_ALIAS} ALIAS ${GRAPHICS_TARGET})
target_sources(${GRAPHICS_TARGET} PRIVATE ${SOURCES} ${HEADERS})

target_link_libraries(${GRAPHICS_TARGET}
    PUBLIC
    glfw
    Core::Glad
    PRIVATE
    ${MODULE_PRIVATE_TARGET}
)

# ================ INPUT =================
set(SOURCES
    Keyboard.cpp
    Mouse.cpp
    Window.cpp
)

set(HEADERS
    Keyboard.h
    Mouse.h
    Window.h
)

add_library(${INPUT_TARGET} STATIC)
add_library(${INPUT_ALIAS} ALIAS ${INPUT_TARGET})
target_sources(${INPUT_TARGET} PRIVATE ${SOURCES} ${HEADERS})

target_link_libraries(${INPUT_TARGET}
    PUBLIC
    Nexium::Foundation
    Nexium::Platform::Graphics
    JustReflectMe::Adapter
    glm
    PRIVATE
    ${MODULE_PRIVATE_TARGET}
)

# =========================================

nexium_configure_target(${MODULE_PRIVATE_TARGET})
nexium_configure_target(${GRAPHICS_TARGET})
nexium_configure_target(${INPUT_TARGET})

add_dependencies(${GRAPHICS_TARGET} CodeGenerator)
add_dependencies(${INPUT_TARGET} CodeGenerator)
