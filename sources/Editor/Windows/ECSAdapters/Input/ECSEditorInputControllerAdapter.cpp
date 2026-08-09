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

#include "ECSEditorInputControllerAdapter.h"

#include "Editor/GuiComponents/Misc.h"
#include "GameplaySystem/Framework/InputController.h"
#include "ImGui/imgui.h"
#include "Misc/IconsFontAwesome.h"

#include <algorithm>
#include <array>
#include <optional>
#include <string>

namespace
{
    template<std::size_t Size>
    void CopyToBuffer(std::array<char, Size>& destination, const Core::StringAtom& source)
    {
        destination.fill('\0');
        const auto& text = source.toStdString();
        std::memcpy(destination.data(), text.data(), std::min(text.size(), Size - 1));
    }

    Core::Keyboard::Key NormalizeModifier(Core::Keyboard::Key key)
    {
        using Key = Core::Keyboard::Key;
        if (key == Key::Right_Shift)
        {
            return Key::Left_Shift;
        }
        if (key == Key::Right_Control)
        {
            return Key::Left_Control;
        }
        if (key == Key::Right_Alt)
        {
            return Key::Left_Alt;
        }
        if (key == Key::Right_Super)
        {
            return Key::Left_Super;
        }
        return key;
    }

    bool IsModifier(Core::Keyboard::Key key)
    {
        using Key = Core::Keyboard::Key;
        return key == Key::Left_Control || key == Key::Left_Shift || key == Key::Left_Alt
               || key == Key::Left_Super;
    }

    std::string_view KeyText(Core::Keyboard::Key key)
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

    std::string ChordText(const Core::KeyChord& chord)
    {
        std::string result;
        const auto append = [&result](std::string_view value)
        {
            if (!result.empty())
            {
                result += " + ";
            }
            result += value;
        };
        for (const auto key : chord.requiredKeys)
        {
            append(KeyText(key));
        }
        if (chord.triggerKey != Core::Keyboard::Key::None)
        {
            append(KeyText(chord.triggerKey));
        }
        return result.empty() ? "None" : result;
    }
} // namespace

namespace Core
{
    ECS_IMPL(ECSEditorInputControllerAdapter);

    bool ECSEditorInputControllerAdapter::canWorkWith(BaseComponent* component) const
    {
        return dynamic_cast<InputController*>(component) != nullptr;
    }

    StringAtom ECSEditorInputControllerAdapter::getProcessedAssetType() const
    {
        return InputController::componentType;
    }

    void ECSEditorInputControllerAdapter::onInitialize()
    {
        ECSEditorMimeAdapter::onInitialize();
    }

    void ECSEditorInputControllerAdapter::onDraw(float)
    {
        if (!Gui::CollapsingHeader("Input shortcuts", ImGuiTreeNodeFlags_DefaultOpen))
        {
            return;
        }

        auto* controller = dynamic_cast<InputController*>(getTargetComponent());
        if (!controller)
        {
            return;
        }

        auto bindings = controller->getBindings();
        bool changed = false;
        std::optional<std::size_t> bindingToDelete;

        for (std::size_t i = 0; i < bindings.size(); ++i)
        {
            auto& binding = bindings[i];
            ImGui::PushID(static_cast<int>(i));

            std::array<char, 128> action{};
            CopyToBuffer(action, binding.action);
            if (ImGui::InputText("Action", action.data(), action.size()))
            {
                binding.action = StringAtom{ action.data() };
                changed = true;
            }

            const bool recording = _recordingBinding && *_recordingBinding == i;
            if (recording)
            {
                const auto& keys = R<Keyboard::Key>::ToArrayC();
                for (const auto rawKey : keys)
                {
                    const auto key = NormalizeModifier(rawKey);
                    if (rawKey == Keyboard::Key::None || rawKey == Keyboard::Key::Last
                        || !Keyboard::IsKeyPressed(rawKey)
                        || std::ranges::find(_recordedKeys, key) != _recordedKeys.end())
                    {
                        continue;
                    }
                    _recordedKeys.push_back(key);
                    if (!IsModifier(key))
                    {
                        _recordedChord.triggerKey = key;
                    }
                }
                _recordedChord.requiredKeys.clear();
                for (const auto key : _recordedKeys)
                {
                    if (key != _recordedChord.triggerKey)
                    {
                        _recordedChord.requiredKeys.push_back(key);
                    }
                }
            }

            const auto shortcutText = ChordText(recording ? _recordedChord : binding.chord);
            ImGui::InputText("Shortcut", const_cast<char*>(shortcutText.c_str()),
                             shortcutText.size() + 1, ImGuiInputTextFlags_ReadOnly);

            if (!recording && ImGui::Button("Record"))
            {
                _recordingBinding = i;
                _recordedChord = {};
                _recordedKeys.clear();
            }
            if (recording)
            {
                ImGui::SameLine();
                ImGui::BeginDisabled(_recordedChord.triggerKey == Keyboard::Key::None);
                if (ImGui::Button("Apply"))
                {
                    binding.chord = _recordedChord;
                    _recordingBinding.reset();
                    changed = true;
                }
                ImGui::EndDisabled();
                ImGui::SameLine();
                if (ImGui::Button("Cancel"))
                {
                    _recordingBinding.reset();
                }
            }

            const char* triggerName
                = binding.trigger == InputActionTrigger::WhileHeld
                      ? "While held"
                      : (binding.trigger == InputActionTrigger::OnRelease ? "On release"
                                                                          : "On press");
            if (ImGui::BeginCombo("Trigger", triggerName))
            {
                if (ImGui::Selectable("While held",
                                      binding.trigger == InputActionTrigger::WhileHeld))
                {
                    binding.trigger = InputActionTrigger::WhileHeld;
                    changed = true;
                }
                if (ImGui::Selectable("On press", binding.trigger == InputActionTrigger::OnPress))
                {
                    binding.trigger = InputActionTrigger::OnPress;
                    changed = true;
                }
                if (ImGui::Selectable("On release",
                                      binding.trigger == InputActionTrigger::OnRelease))
                {
                    binding.trigger = InputActionTrigger::OnRelease;
                    changed = true;
                }
                ImGui::EndCombo();
            }

            if (ImGui::Button(ICON_FA_TRASH " Delete shortcut"))
            {
                bindingToDelete = i;
            }

            ImGui::Separator();
            ImGui::PopID();
        }

        if (bindingToDelete)
        {
            _recordingBinding.reset();
            bindings.erase(bindings.begin() + static_cast<std::ptrdiff_t>(*bindingToDelete));
            changed = true;
        }

        if (ImGui::Button(ICON_FA_PLUS " Add shortcut"))
        {
            bindings.push_back({ .action = "New action"_atom,
                                 .chord = KeyChord::Exact(Keyboard::Key::None),
                                 .trigger = InputActionTrigger::OnPress });
            changed = true;
        }

        if (changed)
        {
            controller->setBindings(std::move(bindings));
            makeParentDirty();
        }
    }
} // namespace Core
