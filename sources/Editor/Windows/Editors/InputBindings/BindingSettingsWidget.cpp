// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "BindingSettingsWidget.h"

#include "Editor/IconsFontAwesome.h"
#include "ImGui/imgui_internal.h"
#include "ImGui/misc/cpp/imgui_stdlib.h"
#include "Platform/Window.h"

#include <algorithm>
#include <array>
#include <utility>

namespace NX
{
    BindingSettingsWidget::DrawResult BindingSettingsWidget::draw(InputController::Binding* binding)
    {
        DrawResult result;
        if (!binding)
        {
            cancelRecording();
        }

        pushPanelStyle();
        if (ImGui::BeginChild("BindingSettingsWidget"_atom.c_str(), { 0.f, 0.f },
                              ImGuiChildFlags_Border))
        {
            if (binding)
            {
                result = drawSettings(*binding);
            }
            else
            {
                ImGui::TextWrapped("%s"_atom.c_str(),
                                   "Select a shortcut to edit its settings"_atom.c_str());
            }
        }
        ImGui::EndChild();
        popPanelStyle();
        return result;
    }

    void BindingSettingsWidget::cancelRecording()
    {
        _recording = false;
        _cancellationRequested = false;
        _recordedChord = {};
        _pressedButtons.clear();
        _captureAreaValid = false;
        _keyRecordingSubscription.release();
        _mouseRecordingSubscription.release();
        _focusRecordingSubscription.release();
        _inputCapture.reset();
        _actionBufferInitialized = false;
    }

    BindingSettingsWidget::DrawResult BindingSettingsWidget::drawSettings(
        InputController::Binding& binding)
    {
        ImGui::TextUnformatted("Shortcut settings"_atom.c_str());
        ImGui::Separator();

        const auto labelWidth = ImGui::CalcTextSize("Shortcut"_atom.c_str()).x;
        ImGui::PushItemWidth(std::max(1.f, ImGui::GetContentRegionAvail().x - labelWidth
                                               - ImGui::GetStyle().ItemInnerSpacing.x));

        DrawResult result;
        ImGui::BeginDisabled(_recording);
        result.changed = drawAction(binding);
        ImGui::EndDisabled();
        result.changed = drawChord(binding) || result.changed;
        ImGui::BeginDisabled(_recording);
        result.changed = drawTrigger(binding) || result.changed;

        ImGui::PopItemWidth();
        ImGui::Separator();
        if (ImGui::Button(ICON_FA_TRASH " Delete shortcut"_atom.c_str()))
        {
            cancelRecording();
            result.deleteRequested = true;
        }
        ImGui::EndDisabled();
        return result;
    }

    bool BindingSettingsWidget::drawAction(InputController::Binding& binding)
    {
        if (!_actionBufferInitialized || _displayedAction != binding.action)
        {
            _actionBuffer = binding.action.toStdString();
            _displayedAction = binding.action;
            _actionBufferInitialized = true;
        }

        if (!ImGui::InputText("Action"_atom.c_str(), &_actionBuffer))
        {
            return false;
        }

        binding.action = Core::StringAtom::Intern(_actionBuffer);
        _displayedAction = binding.action;
        return true;
    }

    bool BindingSettingsWidget::drawChord(InputController::Binding& binding)
    {
        bool changed = false;
        const bool finishedRecording = _recording && _cancellationRequested;
        if (_recording && _cancellationRequested)
        {
            cancelRecording();
        }
        auto shortcutText = chordText(_recording ? _recordedChord : binding.chord).toStdString();
        ImGui::InputText("Shortcut"_atom.c_str(), &shortcutText, ImGuiInputTextFlags_ReadOnly);

        if (!_recording)
        {
            ImGui::BeginDisabled(finishedRecording);
            if (ImGui::Button("Record"_atom.c_str()))
            {
                startRecording();
            }
            ImGui::EndDisabled();
            return changed;
        }

        ImGui::TextUnformatted("Press keys, then confirm with the button below"_atom.c_str());
        drawMouseCaptureArea();
        ImGui::PushItemFlag(ImGuiItemFlags_NoNav, true);
        ImGui::BeginDisabled(_recordedChord.triggerKey.isNone());
        if (ImGui::Button("Confirm"_atom.c_str()))
        {
            binding.chord = _recordedChord;
            cancelRecording();
            changed = true;
        }
        ImGui::EndDisabled();
        ImGui::SameLine();
        if (ImGui::Button("Cancel"_atom.c_str()))
        {
            cancelRecording();
        }
        ImGui::PopItemFlag();
        return changed;
    }

    bool BindingSettingsWidget::drawTrigger(InputController::Binding& binding)
    {
        bool changed = false;
        if (ImGui::BeginCombo("Trigger"_atom.c_str(), triggerText(binding.trigger).c_str()))
        {
            constexpr std::array triggers{ InputActionTrigger::WhileHeld,
                                           InputActionTrigger::OnPress,
                                           InputActionTrigger::OnRelease };
            for (const auto trigger : triggers)
            {
                if (ImGui::Selectable(triggerText(trigger).c_str(), binding.trigger == trigger))
                {
                    binding.trigger = trigger;
                    changed = true;
                }
            }
            ImGui::EndCombo();
        }
        return changed;
    }

    void BindingSettingsWidget::startRecording()
    {
        cancelRecording();
        _recording = true;
        _inputCapture.emplace();
        auto& window = Platform::GetWindow();
        _keyRecordingSubscription = window.onKeyPressed->subscribeAndGetID(
            [this](Platform::Keyboard::Key key, int, Platform::Keyboard::KeyState state,
                   int modifiers)
            {
                if (!_recording || _cancellationRequested
                    || (state != Platform::Keyboard::KeyState::Pressed
                        && state != Platform::Keyboard::KeyState::Released))
                {
                    return;
                }

                recordModifiers(modifiers);
                const auto normalizedKey = normalizeModifier(key);
                if (isModifier(normalizedKey))
                {
                    return;
                }
                if (state == Platform::Keyboard::KeyState::Pressed)
                {
                    recordButton(normalizedKey);
                }
                else
                {
                    std::erase(_pressedButtons, InputButton{ normalizedKey });
                }
            });
        _mouseRecordingSubscription = window.onMouseKeyPressed->subscribeAndGetID(
            [this](Platform::Mouse::Key button, Platform::Mouse::State state,
                   Platform::Mouse::Mod modifiers)
            {
                const auto buttonCode = static_cast<int>(button);
                if (!_recording || _cancellationRequested || buttonCode < 0
                    || buttonCode > GLFW_MOUSE_BUTTON_8)
                {
                    return;
                }

                if (state == Platform::Mouse::State::Release)
                {
                    std::erase(_pressedButtons, InputButton{ button });
                    return;
                }
                if (state != Platform::Mouse::State::Press || !isMouseInCaptureArea())
                {
                    return;
                }

                recordModifiers(static_cast<int>(modifiers));
                recordButton(button);
            });
        _focusRecordingSubscription = window.onFocusChanged->subscribeAndGetID(
            [this](bool focused)
            {
                if (!focused)
                {
                    _cancellationRequested = true;
                }
            });
    }

    void BindingSettingsWidget::recordButton(InputButton button)
    {
        if (button.isNone())
        {
            return;
        }

        if (std::ranges::find(_pressedButtons, button) == _pressedButtons.end())
        {
            _pressedButtons.push_back(button);
        }
        if (button.isMouse() || !isModifier(button.getKeyboardKey()))
        {
            _recordedChord.triggerKey = button;
            _recordedChord.requiredKeys = _pressedButtons;
            std::erase(_recordedChord.requiredKeys, button);
        }
    }

    void BindingSettingsWidget::recordModifiers(int modifiers)
    {
        using Key = Platform::Keyboard::Key;
        constexpr std::array modifierKeys{ std::pair{ GLFW_MOD_CONTROL, Key::Left_Control },
                                           std::pair{ GLFW_MOD_SHIFT, Key::Left_Shift },
                                           std::pair{ GLFW_MOD_ALT, Key::Left_Alt },
                                           std::pair{ GLFW_MOD_SUPER, Key::Left_Super } };
        for (const auto& [modifier, key] : modifierKeys)
        {
            if ((modifiers & modifier) != 0)
            {
                recordButton(key);
            }
            else
            {
                std::erase(_pressedButtons, InputButton{ key });
            }
        }
    }

    void BindingSettingsWidget::drawMouseCaptureArea()
    {
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImGui::GetStyleColorVec4(ImGuiCol_FrameBg));
        if (ImGui::BeginChild("MouseCaptureArea"_atom.c_str(), { 0.f, 72.f },
                              ImGuiChildFlags_Border))
        {
            ImGui::TextWrapped("%s"_atom.c_str(),
                               "Click here to record mouse buttons"_atom.c_str());
        }
        ImGui::EndChild();
        ImGui::PopStyleColor();
        _captureAreaMin = ImGui::GetItemRectMin();
        _captureAreaMax = ImGui::GetItemRectMax();
        _captureAreaValid = true;
    }

    bool BindingSettingsWidget::isMouseInCaptureArea() const
    {
        if (!_captureAreaValid || !ImGui::GetCurrentContext())
        {
            return false;
        }

        const auto position = Platform::GetWindow().getRawWindow() ? Platform::Mouse::GetPosition()
                                                                   : ImGui::GetIO().MousePos;
        return position.x >= _captureAreaMin.x && position.y >= _captureAreaMin.y
               && position.x < _captureAreaMax.x && position.y < _captureAreaMax.y;
    }

    Platform::Keyboard::Key BindingSettingsWidget::normalizeModifier(Platform::Keyboard::Key key)
    {
        using Key = Platform::Keyboard::Key;
        switch (key)
        {
            case Key::Right_Shift:
                return Key::Left_Shift;
            case Key::Right_Control:
                return Key::Left_Control;
            case Key::Right_Alt:
                return Key::Left_Alt;
            case Key::Right_Super:
                return Key::Left_Super;
            default:
                return key;
        }
    }

    bool BindingSettingsWidget::isModifier(Platform::Keyboard::Key key)
    {
        using Key = Platform::Keyboard::Key;
        return key == Key::Left_Control || key == Key::Left_Shift || key == Key::Left_Alt
               || key == Key::Left_Super;
    }

    Core::StringAtom BindingSettingsWidget::buttonText(InputButton button)
    {
        if (button.isMouse())
        {
            using Mouse = Platform::Mouse::Key;
            switch (button.getMouseButton())
            {
                case Mouse::Left:
                    return "Mouse Left"_atom;
                case Mouse::Right:
                    return "Mouse Right"_atom;
                case Mouse::Middle:
                    return "Mouse Middle"_atom;
                default:
                    return "Mouse {}"_f << (static_cast<int>(button.getMouseButton()) + 1);
            }
        }

        const auto key = button.getKeyboardKey();
        using Key = Platform::Keyboard::Key;
        switch (key)
        {
            case Key::Left_Control:
                return "Ctrl"_atom;
            case Key::Left_Shift:
                return "Shift"_atom;
            case Key::Left_Alt:
                return "Alt"_atom;
            case Key::Left_Super:
                return "Super"_atom;
            default:
                return Core::StringAtom::Intern(R<Key>::ToString(key));
        }
    }

    Core::StringAtom BindingSettingsWidget::chordText(const KeyChord& chord)
    {
        Core::StringAtom result;
        const auto append = [&result](const Core::StringAtom& value)
        {
            if (!result.isEmpty())
            {
                result += " + "_atom;
            }
            result += value;
        };
        for (const auto button : chord.requiredKeys)
        {
            append(buttonText(button));
        }
        if (!chord.triggerKey.isNone())
        {
            append(buttonText(chord.triggerKey));
        }
        return result.isEmpty() ? "None"_atom : result;
    }

    Core::StringAtom BindingSettingsWidget::triggerText(InputActionTrigger trigger)
    {
        switch (trigger)
        {
            case InputActionTrigger::WhileHeld:
                return "While held"_atom;
            case InputActionTrigger::OnRelease:
                return "On release"_atom;
            case InputActionTrigger::OnPress:
                return "On press"_atom;
        }
        return "On press"_atom;
    }

    void BindingSettingsWidget::pushPanelStyle()
    {
        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 6.f);
        ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 1.f);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.f);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.f);
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 10.f, 10.f });
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 8.f, 8.f });

        ImGui::PushStyleColor(ImGuiCol_ChildBg, { 0.055f, 0.062f, 0.090f, 1.f });
        ImGui::PushStyleColor(ImGuiCol_Border, { 0.18f, 0.20f, 0.29f, 1.f });
        ImGui::PushStyleColor(ImGuiCol_FrameBg, { 0.075f, 0.085f, 0.125f, 1.f });
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, { 0.095f, 0.105f, 0.155f, 1.f });
        ImGui::PushStyleColor(ImGuiCol_FrameBgActive, { 0.105f, 0.115f, 0.175f, 1.f });
        ImGui::PushStyleColor(ImGuiCol_TextDisabled, { 0.55f, 0.56f, 0.72f, 1.f });
        ImGui::PushStyleColor(ImGuiCol_Button, { 0.322f, 0.188f, 0.624f, 1.f });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.408f, 0.247f, 0.773f, 1.f });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0.255f, 0.149f, 0.510f, 1.f });
    }

    void BindingSettingsWidget::popPanelStyle()
    {
        ImGui::PopStyleColor(9);
        ImGui::PopStyleVar(6);
    }
} // namespace NX
