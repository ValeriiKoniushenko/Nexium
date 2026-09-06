# Add real consumer compile/link/run checks to a build without enabling the
# unmigrated Nexium_Tests target:
# cmake -S . -B build -DCMAKE_PROJECT_Nexium_INCLUDE=<absolute-path-to-this-file>
# cmake --build build --target Nexium_CheckLayers
function(nexium_add_layer_checks)
    set(_directory "${PROJECT_BINARY_DIR}/cmake-layer-checks")
    file(MAKE_DIRECTORY "${_directory}")
    file(WRITE "${_directory}/Foundation.cpp" [=[
#include "Foundation/Configs.h"
#include "Foundation/BaseLog.h"
#include "Foundation/Interfaces/DataStream.h"
int main() { return Foundation::Config::Path::projectAbsPath.empty(); }
]=])
    file(WRITE "${_directory}/Graphics.cpp" [=[
#include "Platform/Graphics.h"
int main() { return glfwGetVersionString() == nullptr; }
]=])
    file(WRITE "${_directory}/Input.cpp" [=[
#include "Platform/Window.h"
#include "Platform/Keyboard.h"
#include "Platform/Mouse.h"
int main() { return !RInternal::GetClassFieldsAsMap({}).empty(); }
]=])
    file(WRITE "${_directory}/RawBackend.cpp" [=[
#include "RawBackend/Image.h"
#include "RawBackend/IDrawable.h"
int main() {
    RawBackend::Image image;
    return !image.isEmpty() || !RInternal::GetClassFieldsAsMap({}).empty();
}
]=])

    foreach(_layer IN ITEMS Foundation Graphics Input RawBackend)
        add_executable(Nexium_Check${_layer} EXCLUDE_FROM_ALL "${_directory}/${_layer}.cpp")
    endforeach()
    target_link_libraries(Nexium_CheckFoundation PRIVATE Nexium::Foundation)
    target_link_libraries(Nexium_CheckGraphics PRIVATE Nexium::Platform::Graphics)
    target_link_libraries(Nexium_CheckInput PRIVATE Nexium::Platform::Input)
    target_link_libraries(Nexium_CheckRawBackend PRIVATE Nexium::RawBackend)

    add_custom_target(Nexium_CheckLayers
        COMMAND "$<TARGET_FILE:Nexium_CheckFoundation>"
        COMMAND "$<TARGET_FILE:Nexium_CheckGraphics>"
        COMMAND "$<TARGET_FILE:Nexium_CheckInput>"
        COMMAND "$<TARGET_FILE:Nexium_CheckRawBackend>"
        DEPENDS Nexium_CheckFoundation Nexium_CheckGraphics Nexium_CheckInput Nexium_CheckRawBackend
        COMMENT "Checking public layer dependencies and runtime linkage"
        VERBATIM
    )
endfunction()

cmake_language(DEFER CALL nexium_add_layer_checks)
