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
#include "Editor/GuiComponents/Misc.h"
#include "Editor/GuiComponents/Separator.h"
#include "Editor/GuiComponents/VecInput.h"
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

            constexpr float labelWidth = 140.0f;
            constexpr float inputWidth = 340.0f;

            using namespace Gui;

            {
                _logicalPath = _mainActorLayout.addChildComponent<LabelRow<TextInput>>(
                    "Logical path", labelWidth);
                _logicalPath->input->setFlex(Flex::FlexWidth);
                _logicalPath->input->setReadOnly(true);
            }

            _actorPostDraw
                = _subActorLayout.addChildComponent<LabelRow<CheckBox>>("Post draw", labelWidth);

            _actorPosition
                = _subActorLayout.addChildComponent<LabelRow<Float3Input>>("Position", labelWidth);
            _actorPosition->input->setWidth(inputWidth);
            _actorPosition->input->setFlex(Flex::Fixed);

            _actorRotation
                = _subActorLayout.addChildComponent<LabelRow<Float3Input>>("Rotation", labelWidth);
            _actorRotation->input->setWidth(inputWidth);
            _actorRotation->input->setFlex(Flex::Fixed);

            _actorScale
                = _subActorLayout.addChildComponent<LabelRow<Float3Input>>("Scale", labelWidth);
            _actorScale->input->setWidth(inputWidth);
            _actorScale->input->setFlex(Flex::Fixed);

            _actorOrigin
                = _subActorLayout.addChildComponent<LabelRow<Float3Input>>("Origin", labelWidth);
            _actorOrigin->input->setWidth(inputWidth);
            _actorOrigin->input->setFlex(Flex::Fixed);
        }

        void onDraw() override
        {
            NxEditorBaseEditorEWC::onDraw();

            const auto dt = GetWorld().timeDelta;
            _mainActorLayout.tick(dt);

            if (Gui::CollapsingHeader("Actor properties", ImGuiTreeNodeFlags_DefaultOpen))
            {
                _subActorLayout.tick(dt);
            }
        }

        void onDiscardChanges() override {}

        void onSave() override {}

        virtual void updateGuiBasedOnAsset()
        {
            if (!_targetActor)
            {
                return;
            }

            _logicalPath->input->setInputtedData(_targetActor->getLogicPath().toStdString());
        }

    protected:
        Gui::VerticalLayout _mainActorLayout;
        Gui::VerticalLayout _subActorLayout;

        Gui::LabelRow<Gui::TextInput>* _logicalPath = nullptr;
        Gui::LabelRow<Gui::CheckBox>* _actorPostDraw = nullptr;
        Gui::LabelRow<Gui::Float3Input>* _actorPosition = nullptr;
        Gui::LabelRow<Gui::Float3Input>* _actorRotation = nullptr;
        Gui::LabelRow<Gui::Float3Input>* _actorScale = nullptr;
        Gui::LabelRow<Gui::Float3Input>* _actorOrigin = nullptr;

        BaseActorAsset<T>* _targetActor = nullptr;
    };

    ECS_TEMPLATE_COMPONENT_IMPL(NxActorBasedEditorEWC<T>, IsActorBased T);

} // namespace Core