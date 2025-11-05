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

#include "Editor/GuiComponents/CheckBox.h"
#include "Editor/GuiComponents/Input.h"
#include "Editor/GuiComponents/LabelRow.h"
#include "Editor/GuiComponents/VecInput.h"
#include "Editor/GuiComponents/VerticalLayout.h"
#include "NxECSBasedEditor.h"

namespace Core
{

    class BaseComponentAdapter : public ECSEditorMimeAdapter
    {
        ECS_COMPONENT_DECL(BaseComponentAdapter, ECSEditorMimeAdapter);

    protected:
        void onApplyAssetData(const nlohmann::json& json) override;
        void onInitialize() override;
        void onDraw(float dt) override;

    protected:
        Gui::VerticalLayout _baseEcsLayout;
        Gui::LabelRow<Gui::TextInput>* _ecsName = nullptr;
        Gui::LabelRow<Gui::TextInput>* _ecsType = nullptr;
        Gui::LabelRow<Gui::CheckBox>* _ecsDisableTicks = nullptr;
        Gui::LabelRow<Gui::CheckBox>* _ecsEnabledComponent = nullptr;
        Gui::LabelRow<Gui::TextInput>* _ecsParent = nullptr;
        Gui::LabelRow<Gui::StringArray>* _ecsChildren = nullptr;
    };

} // namespace Core