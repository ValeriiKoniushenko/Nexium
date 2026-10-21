// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "InputCollector.h"

#include "GLFW/glfw3.h"
#include "Platform/Window.h"

#include <string>
#include <utility>

namespace NX
{
    void InputCollector::initialize(Platform::Window& window, std::function<void()> onFocusLost)
    {
        if (_initialized)
        {
            return;
        }
        _subscriptions << window.onKeyPressed->subscribeAndGetID(
            [this](Platform::Keyboard::Key key, int scancode, Platform::Keyboard::KeyState state,
                   int mods) { pushKeyEvent(key, scancode, state, mods); });
        _subscriptions << window.onMouseKeyPressed->subscribeAndGetID(
            [this](Platform::Mouse::Key button, Platform::Mouse::State state,
                   Platform::Mouse::Mod mods) { pushMouseEvent(button, state, mods); });
        _subscriptions << window.onMouseMove->subscribeAndGetID([this](glm::vec2 position)
                                                                { pushPointerEvent(position); });
        _subscriptions << window.onMouseWheel->subscribeAndGetID([this](glm::vec2 offset)
                                                                 { pushWheelEvent(offset); });
        _subscriptions << window.onFocusChanged->subscribeAndGetID(
            [callback = std::move(onFocusLost)](bool focused)
            {
                if (!focused)
                {
                    callback();
                }
            });
        _initialized = true;
    }

    void InputCollector::pushKeyEvent(Platform::Keyboard::Key key, int scancode,
                                      Platform::Keyboard::KeyState state, int mods)
    {
        if (key == Platform::Keyboard::Key::None)
        {
            return;
        }
        _events.emplace_back(KeyInputEvent{ .key = key,
                                            .state = state,
                                            .modifiers = ConvertModifiers(mods),
                                            .scancode = scancode,
                                            .pressedKeys = {} });
    }

    void InputCollector::pushMouseEvent(Platform::Mouse::Key button, Platform::Mouse::State state,
                                        Platform::Mouse::Mod mods)
    {
        if (button == Platform::Mouse::Key::None
            || (state != Platform::Mouse::State::Press && state != Platform::Mouse::State::Release))
        {
            return;
        }

        _events.emplace_back(KeyInputEvent{ .key = button,
                                            .state = state == Platform::Mouse::State::Press
                                                         ? Platform::Keyboard::KeyState::Pressed
                                                         : Platform::Keyboard::KeyState::Released,
                                            .modifiers = ConvertModifiers(static_cast<int>(mods)),
                                            .pressedKeys = {} });
    }

    std::uint8_t InputCollector::ConvertModifiers(int mods)
    {
        std::uint8_t result = InputModifier_None;
        if (mods & GLFW_MOD_SHIFT)
        {
            result |= NX::InputModifier_Shift;
        }
        if (mods & GLFW_MOD_CONTROL)
        {
            result |= NX::InputModifier_Control;
        }
        if (mods & GLFW_MOD_ALT)
        {
            result |= NX::InputModifier_Alt;
        }
        if (mods & GLFW_MOD_SUPER)
        {
            result |= NX::InputModifier_Super;
        }
        return result;
    }

    void InputCollector::pushPointerEvent(glm::vec2 position)
    {
        _events.emplace_back(
            KeyInputEvent{ .type = InputEventType::PointerMove, .position = position });
    }

    void InputCollector::pushWheelEvent(glm::vec2 offset)
    {
        _events.emplace_back(KeyInputEvent{ .type = InputEventType::Wheel, .value = offset });
    }

    bool InputCollector::pollEvent(KeyInputEvent& event)
    {
        if (_events.empty())
        {
            return false;
        }
        event = std::move(_events.front());
        _events.pop_front();
        return true;
    }

    void InputCollector::reset()
    {
        _events.clear();
    }

} // namespace NX
