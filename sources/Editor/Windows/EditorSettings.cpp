/*
 * MIT License
 *
 * Copyright (c) 2018-2027 Valerii Koniushenko
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
#include "Editor/GuiComponents/HorizontalLayout.h"
#include "Editor/GuiComponents/Input.h"
#include "Editor/GuiComponents/Label.h"
#include "Editor/GuiComponents/Separator.h"
#include "Editor/GuiComponents/Spacer.h"
#include "GameplaySystem/Framework/GameInstance.h"
#include "GameplaySystem/Framework/InputController.h"
#include "Misc/IconsFontAwesome.h"
#include "Scene/Spectator.h"

#include <string>
#include <string_view>

using namespace Core::Gui;

namespace
{
    std::string_view GetKeyLabel(Core::Keyboard::Key key)
    {
        using Key = Core::Keyboard::Key;
        if (key == Key::Left_Control)
        {
            return "Ctrl";
        }
        if (key == Key::Left_Shift)
        {
            return "Shift";
        }
        if (key == Key::Left_Alt)
        {
            return "Alt";
        }
        if (key == Key::Left_Super)
        {
            return "Super";
        }
        return R<Key>::ToString(key);
    }

    Core::StringAtom GetChordLabel(const Core::KeyChord& chord)
    {
        std::string result;
        const auto append = [&result](std::string_view key)
        {
            if (!result.empty())
            {
                result += " + ";
            }
            result += key;
        };

        for (const auto key : chord.requiredKeys)
        {
            append(GetKeyLabel(key));
        }
        if (chord.triggerKey != Core::Keyboard::Key::None)
        {
            append(GetKeyLabel(chord.triggerKey));
        }

        return Core::StringAtom::Intern(result);
    }

    std::string_view GetTriggerLabel(Core::InputActionTrigger trigger)
    {
        switch (trigger)
        {
            case Core::InputActionTrigger::WhileHeld:
                return "While held";
            case Core::InputActionTrigger::OnPress:
                return "On press";
            case Core::InputActionTrigger::OnRelease:
                return "On release";
        }
        return "Unknown";
    }
} // namespace

namespace Core
{
    ECS_IMPL(EditorSettingsEWC);
    ECS_IMPL(Internal::BaseListItem);
    ECS_IMPL(Internal::KeymapItem);
    ECS_IMPL(Internal::ColorItem);

    const char* EditorSettingsEWC::getIcon()
    {
        return ICON_FA_COG;
    }

    bool Internal::BaseListItem::containsString(const StringAtom& str)
    {
        if (Verify(_label))
        {
            Assert(!_label->getComponentName().isEmpty());

            if (_label->getComponentName().findIgnoreCase(str) != nullptr)
            {
                return true;
            }
        }

        return false;
    }

    void Internal::BaseListItem::setLabel(const StringAtom& label)
    {
        if (Verify(_label))
        {
            _label->setText(label);
        }
    }

    void Internal::BaseListItem::onInitialize()
    {
        HorizontalLayout::onInitialize();

        setComponentName("ListItem");
        setFlex(Flex::FlexWidth);
        setHorizontalAlign(Align::SpaceBetween);
        _label = addChildComponent<Label>("Undefined");
    }

    void Internal::KeymapItem::setReadOnly(bool value)
    {
        if (Verify(_button))
        {
            _button->disableWidget(value);
            _resetButton->disableWidget(value);
        }
    }

    bool Internal::KeymapItem::containsString(const StringAtom& str)
    {
        if (BaseListItem::containsString(str))
        {
            return true;
        }

        if (Verify(_button))
        {
            if (_button->getComponentName().findIgnoreCase(str) != nullptr)
            {
                return true;
            }
        }

        return false;
    }

    void Internal::KeymapItem::setButtonName(const StringAtom& label)
    {
        if (Verify(_button))
        {
            _button->setText(label);
        }
    }

    void Internal::KeymapItem::onInitialize()
    {
        BaseListItem::onInitialize();

        auto* holder = addChildComponent<HorizontalLayout>();
        holder->setFlex(Flex::Fixed);
        _button = holder->addChildComponent<Button>("None");
        _button->setMinWidth(80.f);

        _resetButton = holder->addChildComponent<Button>(ICON_FA_SHARE);
    }

    void Internal::ColorItem::setReadOnly(bool value)
    {
        if (Verify(_colorInput))
        {
            _colorInput->setReadOnly(value);
        }
    }

    void Internal::ColorItem::setInputData(const StringAtom& data)
    {
        if (Verify(_colorInput))
        {
            _colorInput->setInputtedData(data.data());
        }
    }

    void Internal::ColorItem::onInitialize()
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
        const float tick = gGameInstance->world.getTimeDelta();

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
        auto gap = ImGui::GetStyle().WindowPadding.x;
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

        for (const auto& [fst, snd] : GetEditor()->keyboardInput.getMapping())
        {
            auto* item = layout.addChildComponent<Internal::KeymapItem>();
            item->setReadOnly(true);
            item->setLabel(fst);
            item->setButtonName(
                R<Keyboard::Key>::ToString(snd->getKey().value_or(Keyboard::Key::None)).data());
        }

        // static keys
        {
            auto* item = layout.addChildComponent<Internal::KeymapItem>();
            item->setReadOnly(true);
            item->setLabel("Show ImGui debug rects");
            item->setButtonName(
                R<Keyboard::Key>::ToString(Widget::Input::editorImGuiShowRect).data());
        }

        if (auto spectator = gGameInstance->gameScene.gerFirstOf<Spectator>())
        {
            if (const auto* input = spectator->findFirstChildOf<InputController>())
            {
                layout.addChildComponent<Spacer>();
                layout.addChildComponent<Label>()->setText("Spectator");
                layout.addChildComponent<Separator>();

                for (const auto& binding : input->getBindings())
                {
                    auto* item = layout.addChildComponent<Internal::KeymapItem>();
                    item->setReadOnly(true);
                    item->setLabel(StringAtom::Intern(binding.action.toStdString() + " ("
                                                      + GetTriggerLabel(binding.trigger).data()
                                                      + ")"));
                    item->setButtonName(GetChordLabel(binding.chord));
                }
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
        if (!Verify(style))
        {
            errorLog("Can't setup ImGUI styles. ImGui::GetStyle() return nullptr");
            return;
        }

        auto& colors = style->Colors;

        for (int i = 0; i < ImGuiCol_COUNT; ++i)
        {
            auto* color = layout.addChildComponent<Internal::ColorItem>();
            color->setLabel(ImGui::GetStyleColorName(i));
            color->setReadOnly(true);
            auto data = Core::Color4::From(NormColor4(colors[i]));
            color->setInputData("{},{},{},{}"_f << data.x << data.y << data.z << data.w);
        }
    }
} // namespace Core
