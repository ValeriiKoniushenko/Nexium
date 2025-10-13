/*
 * MIT License
 *
 * Copyright (c) 2018-2025 Valerii Koniushenko
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#pragma once

#include "AssetsManager/Mesh3DAsset.h"
#include "BaseWindow.h"
#include "Editor/GuiComponents/Array.h"
#include "Editor/GuiComponents/Button.h"
#include "Editor/GuiComponents/CheckBox.h"
#include "Editor/GuiComponents/Combo.h"
#include "Editor/GuiComponents/HorizontalLayout.h"
#include "Editor/GuiComponents/Input.h"
#include "Editor/GuiComponents/LabelRow.h"
#include "Editor/GuiComponents/VerticalLayout.h"
#include "GameplaySystem/Framework/GameInstance.h"
#include "NxEditorBaseEditor.h"

namespace Core
{

    template<IsActorBased T>
    class NxActorBasedEditorEWC : public NxEditorBaseEditorEWC
    {
        ECS_TEMPLATE_COMPONENT_DECL(NxActorBasedEditorEWC, NxEditorBaseEditorEWC, T);

    public:
    protected:
        void onInitialize() override
        {
            NxEditorBaseEditorEWC::onInitialize();

            constexpr float defaultLabelWidth = 140.0f;
            constexpr float defaultModifierWidth = 300.0f;

            using namespace Gui;

            {
                _actorLayout.addChildComponent<LabelRow<CheckBox>>("Post draw");
            }
        }

        void onDraw() override
        {
            NxEditorBaseEditorEWC::onDraw();

            _actorLayout.tick(GetWorld().timeDelta);
        }

        void onDiscardChanges() override {}

        void onSave() override {}

    protected:
        Gui::VerticalLayout _actorLayout;

        BaseActorAsset<T>* _targetActor = nullptr;
    };

    ECS_TEMPLATE_COMPONENT_IMPL(NxActorBasedEditorEWC<T>, IsActorBased T);

} // namespace Core