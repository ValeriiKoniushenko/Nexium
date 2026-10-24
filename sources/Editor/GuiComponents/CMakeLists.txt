file(GLOB EDITOR_GUI_SOURCES CONFIGURE_DEPENDS "*.cpp")
file(GLOB EDITOR_GUI_HEADERS CONFIGURE_DEPENDS "*.h")
foreach(EDITOR_GUI_HEADER IN LISTS EDITOR_GUI_HEADERS)
    if(EDITOR_GUI_HEADER MATCHES "\\.generated\\.h$")
        set_source_files_properties("${EDITOR_GUI_HEADER}" PROPERTIES SKIP_LINTING TRUE)
    endif()
endforeach()

add_library(Nexium_Editor_GuiComponents STATIC)
add_library(Nexium::Editor::GuiComponents ALIAS Nexium_Editor_GuiComponents)
target_sources(Nexium_Editor_GuiComponents
    PRIVATE ${EDITOR_GUI_SOURCES}
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS "${PROJECT_SOURCE_DIR}/sources"
        FILES ${EDITOR_GUI_HEADERS}
)
target_link_libraries(Nexium_Editor_GuiComponents
    PUBLIC
    Core::ImGui
    Nexium::Foundation::Base
    Nexium::Fundamental::ECS
    Nexium::Fundamental::ResourceManagement
    Nexium::Platform::Input
    Nexium::Subsystem::AssetsManager
    Utils::Core
    Utils::Utils
)
target_precompile_headers(Nexium_Editor_GuiComponents PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../EditorPCH.h")

nexium_configure_target(Nexium_Editor_GuiComponents)
add_dependencies(Nexium_Editor_GuiComponents CodeGenerator)
