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
#include "Editor/GuiComponents/Separator.h"
#include "Editor/GuiComponents/Spacer.h"
#include "GameplaySystem/Framework/GameInstance.h"

using namespace Core::Gui;

namespace Core
{
    ECS_COMPONENT_IMPL(EditorSettingsEWC)

    void EditorSettingsEWC::onInitialize()
    {
        BaseFloatEWC::onInitialize();

        setupCommonLayoutSettings();
        createPage_Keymap();
    }

    void EditorSettingsEWC::onDraw()
    {
        const float tick = gGameInstance->world.timeDelta;

        if (ImGui::BeginChild("MainMenu", glm::vec2(120.f, 0), ImGuiChildFlags_ResizeX))
        {
            drawSettingsTree();
        }
        ImGui::EndChild();

        ImGui::SameLine();

        if (ImGui::BeginChild("SettingChild"))
        {
            _layouts[_currentMenu].tick(tick);
        }
        ImGui::EndChild();
    }

    void EditorSettingsEWC::drawSettingsTree()
    {
        if (ImGui::TreeNodeEx("Keymap",
                              ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_SpanAvailWidth))
        {
            ImGui::TreePop();
        }
        if (ImGui::IsItemClicked() || (ImGui::IsItemFocused() && isHovered()))
        {
            _currentMenu = Menu_Keymap;
        }

        if (ImGui::TreeNodeEx("About", ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_SpanAvailWidth))
        {
            ImGui::TreePop();
        }
        if (ImGui::IsItemClicked() || (ImGui::IsItemFocused() && isHovered()))
        {
            _currentMenu = Menu_About;
        }
    }

    void EditorSettingsEWC::setupCommonLayoutSettings()
    {
        const auto gap = ImGui::GetStyle().WindowPadding.x;
        for (auto& layout : _layouts)
        {
            layout.setPaddings(gap, gap, 0, 0);
        }
    }

    void EditorSettingsEWC::createPage_Keymap()
    {
        auto& layout = _layouts[Menu_Keymap];

        layout.addChildComponent<Spacer>();
        layout.addChildComponent<Label>()->setText("General");
        layout.addChildComponent<Separator>();

        auto line = HorizontalLayout::Create();
        line->setComponentName("Label-Key line holder");
        line->setFlex(Flex::FlexWidth);
        line->setHorizontalAlign(Align::SpaceBetween);
        line->addChildComponent<Label>("Switch Editor/Game mode");
        auto* holder = line->addChildComponent<HorizontalLayout>();
        holder->setComponentName("Button changer + reset");
        holder->setFlex(Flex::Fixed);
        holder->addChildComponent<Button>("None");
        holder->addChildComponent<Button>(ICON_FA_SHARE);

        layout.attachChild(line);
        layout.attachChild(line);
        layout.attachChild(line);
    }

} // namespace Core
