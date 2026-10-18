// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "InputTypes.h"

#include <algorithm>
#include <ranges>

namespace NX
{
    InputButton NormalizeInputButton(InputButton button) noexcept
    {
        if (button.isMouse())
        {
            return button;
        }
        using Key = Platform::Keyboard::Key;
        switch (button.getKeyboardKey())
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
                return button;
        }
    }

    bool IsInputModifier(InputButton button) noexcept
    {
        if (button.isMouse())
        {
            return false;
        }
        using Key = Platform::Keyboard::Key;
        const auto key = NormalizeInputButton(button).getKeyboardKey();
        return key == Key::Left_Shift || key == Key::Left_Control || key == Key::Left_Alt
               || key == Key::Left_Super;
    }

    std::size_t InputCapture::_captureCount = 0;
    bool InputCapture::_awaitingRelease = false;
    std::vector<InputButton> InputCapture::_heldButtons;

    InputCapture::InputCapture()
    {
        ++_captureCount;
    }

    InputCapture::~InputCapture()
    {
        if (--_captureCount == 0)
        {
            _awaitingRelease = !_heldButtons.empty();
        }
    }

    bool InputCapture::isActive() noexcept
    {
        return _captureCount != 0 || _awaitingRelease;
    }

    void InputCapture::updateButton(InputButton button, Platform::Keyboard::KeyState state)
    {
        if (button.isNone())
        {
            return;
        }
        if (state == Platform::Keyboard::KeyState::Pressed
            && std::ranges::find(_heldButtons, button) == _heldButtons.end())
        {
            _heldButtons.push_back(button);
        }
        else if (state == Platform::Keyboard::KeyState::Released)
        {
            std::erase(_heldButtons, button);
            if (_heldButtons.empty())
            {
                _awaitingRelease = false;
            }
        }
    }

    void InputCapture::resetButtons()
    {
        _heldButtons.clear();
        _awaitingRelease = false;
    }

    KeyChord KeyChord::Exact(InputButton key)
    {
        return { .triggerKey = key, .requiredKeys = {} };
    }

    bool KeyChord::matches(InputButton eventKey, const std::vector<InputButton>& pressedKeys) const
    {
        if (NormalizeInputButton(triggerKey) != NormalizeInputButton(eventKey))
        {
            return false;
        }

        return std::ranges::all_of(
            requiredKeys,
            [&pressedKeys](InputButton key)
            {
                return std::ranges::any_of(
                    pressedKeys, [key](InputButton pressed)
                    { return NormalizeInputButton(pressed) == NormalizeInputButton(key); });
            });
    }

    bool KeyChord::contains(InputButton key) const
    {
        return NormalizeInputButton(triggerKey) == NormalizeInputButton(key)
               || std::ranges::any_of(
                   requiredKeys, [key](InputButton required)
                   { return NormalizeInputButton(required) == NormalizeInputButton(key); });
    }
} // namespace NX
