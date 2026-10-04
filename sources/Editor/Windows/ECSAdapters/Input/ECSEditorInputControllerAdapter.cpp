// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "ECSEditorInputControllerAdapter.h"

#include "Editor/EditorIntegration.h"
#include "Editor/GuiComponents/Misc.h"
#include "Editor/IconsFontAwesome.h"
#include "Editor/Windows/Editors/InputBindings/InputBindingsEditor.h"
#include "ImGui/imgui.h"
#include "NxWorld/Framework/InputController.h"

namespace NX
{
    ECS_IMPL(ECSEditorInputControllerAdapter);

    bool ECSEditorInputControllerAdapter::canWorkWith(BaseComponent* component) const
    {
        return component && component->isTypeOf<InputController>();
    }

    Core::StringAtom ECSEditorInputControllerAdapter::getProcessedAssetType() const
    {
        return InputController::componentType;
    }

    void ECSEditorInputControllerAdapter::onDraw(float)
    {
        if (!Gui::CollapsingHeader("Input shortcuts"_atom.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
        {
            return;
        }

        auto* controller = dynamic_cast<InputController*>(getTargetComponent());
        if (!controller)
        {
            return;
        }

        ImGui::TextUnformatted(("{} shortcuts"_f << controller->getBindings().size()).c_str());
        if (ImGui::Button(ICON_FA_KEYBOARD_O " Edit shortcuts"_atom.c_str()))
        {
            auto* editor = GetEditor();
            if (!editor)
            {
                return;
            }
            if (auto* window = editor->getWindow<InputBindingsEditor>())
            {
                window->setTarget(controller, getParentAs<NxECSBasedEditorEWC>());
                window->openWindow();
            }
        }
    }
} // namespace NX
