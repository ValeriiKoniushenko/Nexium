#include "InputBindingsEditor.h"

#include "ImGui/imgui.h"

namespace NX
{
    ECS_IMPL(InputBindingsEditor);

    void InputBindingsEditor::onInitialize()
    {
        BaseFloatEWC::onInitialize();
        setComponentName("Input bindings"_atom);

        _minWindowSize = FSize2(500.f, 500.f);
        _windowFlags |= ImGuiWindowFlags_MenuBar;

        _bindingsList.initialize();
    }

    void InputBindingsEditor::onDraw()
    {
        _bindingsList.draw();
    }
} // namespace NX
