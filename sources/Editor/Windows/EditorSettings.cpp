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
#include "Scene/Spectator.h"

using namespace Core::Gui;

namespace Core
{
    ECS_COMPONENT_IMPL(EditorSettingsEWC)
    ECS_COMPONENT_IMPL(EditorSettingsEWC::BaseListItem)
    ECS_COMPONENT_IMPL(EditorSettingsEWC::KeymapItem)
    ECS_COMPONENT_IMPL(EditorSettingsEWC::ColorItem)

    bool EditorSettingsEWC::BaseListItem::containsString(const StringAtom& str)
    {
        if (DEBUG_ASSERT_VAL(_label))
        {
            DEBUG_ASSERT(!_label->getComponentName().isEmpty());

            if (_label->getComponentName().findIgnoreCase(str) != nullptr)
            {
                return true;
            }
        }

        return false;
    }

    void EditorSettingsEWC::BaseListItem::setLabel(const StringAtom& label)
    {
        if (DEBUG_ASSERT_VAL(_label))
        {
            _label->setText(label);
        }
    }

    void EditorSettingsEWC::BaseListItem::onInitialize()
    {
        HorizontalLayout::onInitialize();

        setComponentName("ListItem");
        setFlex(Flex::FlexWidth);
        setHorizontalAlign(Align::SpaceBetween);
        _label = addChildComponent<Label>("Undefined");
    }

    void EditorSettingsEWC::KeymapItem::setReadOnly(bool value)
    {
        if (DEBUG_ASSERT_VAL(_button))
        {
            _button->disableWidget(value);
            _resetButton->disableWidget(value);
        }
    }

    bool EditorSettingsEWC::KeymapItem::containsString(const StringAtom& str)
    {
        if (BaseListItem::containsString(str))
        {
            return true;
        }

        if (DEBUG_ASSERT_VAL(_button))
        {
            if (_button->getComponentName().findIgnoreCase(str) != nullptr)
            {
                return true;
            }
        }

        return false;
    }

    void EditorSettingsEWC::KeymapItem::setButtonName(const StringAtom& label)
    {
        if (DEBUG_ASSERT_VAL(_button))
        {
            _button->setText(label);
        }
    }

    void EditorSettingsEWC::KeymapItem::onInitialize()
    {
        BaseListItem::onInitialize();

        auto* holder = addChildComponent<HorizontalLayout>();
        holder->setFlex(Flex::Fixed);
        _button = holder->addChildComponent<Button>("None");
        _button->setMinWidth(80.f);

        _resetButton = holder->addChildComponent<Button>(ICON_FA_SHARE);
    }

    void EditorSettingsEWC::ColorItem::setReadOnly(bool value)
    {
        if (DEBUG_ASSERT_VAL(_colorInput))
        {
            _colorInput->setReadOnly(value);
        }
    }

    void EditorSettingsEWC::ColorItem::setInputData(const StringAtom& data)
    {
        if (DEBUG_ASSERT_VAL(_colorInput))
        {
            _colorInput->setInputtedData(data.data());
        }
    }

    void EditorSettingsEWC::ColorItem::onInitialize()
    {
        BaseListItem::onInitialize();

        auto* holder = addChildComponent<HorizontalLayout>();
        holder->setFlex(Flex::Fixed);
        _colorInput = holder->addChildComponent<TextInput>();
        _colorInput->setWidth(130.f);
    }

    void EditorSettingsEWC::onOpen()
    {
        setupCommonLayoutSettings();
        createPage_Appearance();
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
        int flag = _defaultTreeNodeFlags | ImGuiTreeNodeFlags_Leaf;

        if (ImGui::TreeNodeEx(
                "Appearance",
                flag | (_currentMenu == Menu_Appearance ? ImGuiTreeNodeFlags_Selected : 0)))
        {
            ImGui::TreePop();
        }
        if (ImGui::IsItemClicked() || (ImGui::IsItemFocused() && isHovered()))
        {
            _currentMenu = Menu_Appearance;
        }

        if (ImGui::TreeNodeEx(
                "Keymap", flag | (_currentMenu == Menu_Keymap ? ImGuiTreeNodeFlags_Selected : 0)))
        {
            ImGui::TreePop();
        }
        if (ImGui::IsItemClicked() || (ImGui::IsItemFocused() && isHovered()))
        {
            _currentMenu = Menu_Keymap;
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
        layout.clear();

        layout.addChildComponent<Spacer>();
        layout.addChildComponent<Label>()->setText("General");
        layout.addChildComponent<Separator>();

        for (const auto& [fst, snd] : GetEditor().keyboardInput.getMapping())
        {
            const auto item = layout.addChildComponent<KeymapItem>();
            item->setReadOnly(true);
            item->setLabel(fst);
            item->setButtonName(Keyboard::KeyToString(snd->getKey().value_or(Keyboard::Key_None)));
        }

        // static keys
        {
            const auto item = layout.addChildComponent<KeymapItem>();
            item->setReadOnly(true);
            item->setLabel("Show ImGui debug rects");
            item->setButtonName(Keyboard::KeyToString(Config::Input::editorImGuiShowRect));
        }

        if (auto spectator = gGameInstance->gameScene.getFirstActorOf<Spectator>();
            DEBUG_ASSERT_VAL(spectator))
        {
            layout.addChildComponent<Spacer>();
            layout.addChildComponent<Label>()->setText("Spectator");
            layout.addChildComponent<Separator>();

            for (const auto& [fst, snd] : spectator->keyboardInput.getMapping())
            {
                const auto item = layout.addChildComponent<KeymapItem>();
                item->setReadOnly(true);
                item->setLabel(fst + (snd->onPress.isEmpty() ? "(disabled)" : ""));
                item->setButtonName(
                    Keyboard::KeyToString(snd->getKey().value_or(Keyboard::Key_None)));
            }
        }
    }

    void EditorSettingsEWC::createPage_Appearance()
    {
        auto& layout = _layouts[Menu_Appearance];
        layout.clear();

        layout.addChildComponent<Spacer>();
        layout.addChildComponent<Label>()->setText("Color pallet");
        layout.addChildComponent<Separator>();

        ImGuiStyle* style = &ImGui::GetStyle();
        if (!DEBUG_ASSERT_VAL(style))
        {
            errorLog("Can't setup ImGUI styles. ImGui::GetStyle() return nullptr");
            return;
        }

        glm::vec4* colors = style->Colors;
        if (!DEBUG_ASSERT_VAL(colors))
        {
            errorLog("Can't setup ImGUI colors. ImGui::GetStyle()->Colors return nullptr");
            return;
        }

        for (int i = 0; i < ImGuiCol_COUNT; ++i)
        {
            auto color = layout.addChildComponent<ColorItem>();
            color->setLabel(ImGui::GetStyleColorName(i));
            color->setReadOnly(true);
            auto data = Core::Color4::From(NormColor4(colors[i]));
            color->setInputData("{},{},{},{}"_f << data.x << data.y << data.z << data.w);
        }
    }
} // namespace Core