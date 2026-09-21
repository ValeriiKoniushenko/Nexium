get_filename_component(MODULE_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
set(MODULE_TARGET Nexium_${MODULE_NAME})
set(MODULE_PRIVATE_TARGET ${MODULE_TARGET}_PrivateModule)
set(MODULE_ALIAS Nexium::${MODULE_NAME})

# ============== Per Module Private Info ===============

add_library(${MODULE_PRIVATE_TARGET} STATIC)
target_sources(${MODULE_PRIVATE_TARGET} PRIVATE
    PrivateModuleInfo.cpp
)

target_link_libraries(${MODULE_PRIVATE_TARGET} PRIVATE spdlog::spdlog)
target_compile_definitions(${MODULE_PRIVATE_TARGET} PRIVATE PRIVATE_MODULE_NAME="${MODULE_NAME}")

nexium_configure_target(${MODULE_PRIVATE_TARGET})

# ======================= Editor =======================

file(GLOB_RECURSE EDITOR_SOURCES CONFIGURE_DEPENDS "*.cpp")
file(GLOB_RECURSE EDITOR_HEADERS CONFIGURE_DEPENDS "*.h")
list(REMOVE_ITEM EDITOR_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/PrivateModuleInfo.cpp")
list(REMOVE_ITEM EDITOR_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/PrivateModuleInfo.h")
list(REMOVE_ITEM EDITOR_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/ImGuizmoIntegration.h")
foreach(EDITOR_HEADER IN LISTS EDITOR_HEADERS)
    if(EDITOR_HEADER MATCHES "\\.generated\\.h$")
        set_source_files_properties("${EDITOR_HEADER}" PROPERTIES SKIP_LINTING TRUE)
    endif()
endforeach()

if(NOT NEXIUM_ENABLE_3D_MODULE)
    list(REMOVE_ITEM EDITOR_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/Gizmo.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/Windows/ECSAdapters/EditorStaticMeshAdapter.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/Windows/ECSAdapters/EditorStaticMeshBundleAdapter.cpp"
    )
    list(FILTER EDITOR_HEADERS EXCLUDE REGEX
        "/(Gizmo|EditorStaticMeshAdapter|EditorStaticMeshBundleAdapter)(\\.generated)?\\.h$"
    )
endif()

add_library(Nexium_Editor STATIC)
add_library(Nexium::Editor ALIAS Nexium_Editor)
target_sources(Nexium_Editor
    PRIVATE ${EDITOR_SOURCES}
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS "${PROJECT_SOURCE_DIR}/sources"
        FILES ${EDITOR_HEADERS}
)
target_link_libraries(Nexium_Editor
    PUBLIC
        Nexium::Runtime
        Core::ImGui
    PRIVATE ${MODULE_PRIVATE_TARGET} imguizmo::imguizmo
)

if(NEXIUM_ENABLE_3D_MODULE)
    target_compile_definitions(Nexium_Editor PUBLIC NEXIUM_ENABLE_3D_MODULE=1)
endif()

nexium_configure_target(Nexium_Editor)
add_dependencies(Nexium_Editor CodeGenerator)
