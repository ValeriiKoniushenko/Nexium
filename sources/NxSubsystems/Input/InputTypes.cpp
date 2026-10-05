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
        if (triggerKey != eventKey)
        {
            return false;
        }

        return std::ranges::all_of(
            requiredKeys, [&pressedKeys](InputButton key)
            { return std::ranges::find(pressedKeys, key) != pressedKeys.end(); });
    }

    bool KeyChord::contains(InputButton key) const
    {
        return triggerKey == key || std::ranges::find(requiredKeys, key) != requiredKeys.end();
    }
} // namespace NX
