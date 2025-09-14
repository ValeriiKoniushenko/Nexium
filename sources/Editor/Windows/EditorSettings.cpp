// MIT License
//
// Copyright (c) 2019-2025 Valerii Koniushenko
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "EditorSettings.h"

#include "Editor/GuiComponents/Button.h"
#include "Editor/GuiComponents/CheckBox.h"
#include "Editor/GuiComponents/Combo.h"
#include "Editor/GuiComponents/HorizontalLayout.h"
#include "Editor/GuiComponents/Input.h"
#include "Editor/GuiComponents/Label.h"
#include "Editor/GuiComponents/List.h"
#include "GameplaySystem/Framework/GameInstance.h"

using namespace Core::Gui;

namespace Core
{
    ECS_COMPONENT_IMPL(EditorSettingsEWC)

    void EditorSettingsEWC::onInitialize()
    {
        BaseFloatEWC::onInitialize();

        _layout.addChildComponent<ListView>()->setFlex(Flex::FlexWidth);

        const auto search = HorizontalLayout::Create();
        search->setHorizontalAlign(Align::Center);
        search->addChildComponent<Label>("Search:");
        search->addChildComponent<TextInput>()->setFlex(Flex::FlexWidth);
        search->addChildComponent<CheckBox>();
        search->addChildComponent<ToggleButton>();

        _layout.attachChild(search);
        _layout.attachChild(search);

        _texture.loadFromFile(Config::Path::images / "redcolor.png");
    }

    void EditorSettingsEWC::onDraw()
    {
        const float tick = gGameInstance->world.timeDelta;

        auto s = ImGui::GetCursorPosX();
        ImGui::ImageButton("SMTH1", _texture.getTextureId(), { 64, 64 });
        ImGui::SameLine();
        auto s1 = ImGui::GetCursorPosX();
        ImGui::ImageButton("SMTH2", _texture.getTextureId(), { 64, 64 });
        auto s2 = ImGui::GetCursorPosX();

        int i = 1;
    }
} // namespace Core
