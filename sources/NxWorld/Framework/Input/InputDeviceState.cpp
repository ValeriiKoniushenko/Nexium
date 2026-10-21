// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "InputDeviceState.h"

#include <algorithm>

namespace NX
{
    KeyInputEvent InputDeviceState::apply(KeyInputEvent event)
    {
        using KeyState = Platform::Keyboard::KeyState;

        switch (event.type)
        {
            case InputEventType::Button:
            {
                if (event.state == KeyState::Pressed
                    && !std::ranges::contains(_heldButtons, event.key))
                {
                    _heldButtons.push_back(event.key);
                }
                else if (event.state == KeyState::Released)
                {
                    std::erase(_heldButtons, event.key);
                }

                event.key = NormalizeInputButton(event.key);
                break;
            }

            case InputEventType::PointerMove:
            {
                event.value = event.position - _pointerPosition.value_or(event.position);

                _pointerPosition = event.position;
                break;
            }

            default:
                break;
        }

        if (event.type != InputEventType::PointerMove)
        {
            event.position = _pointerPosition.value_or(glm::vec2{});
        }

        event.pressedKeys = getChordKeys();
        event.modifiers = getModifiers();

        return event;
    }

    std::vector<InputButton> InputDeviceState::getChordKeys() const
    {
        std::vector<InputButton> result;
        for (const auto button : _heldButtons)
        {
            const auto normalized = NormalizeInputButton(button);
            if (std::ranges::find(result, normalized) == result.end())
            {
                result.push_back(normalized);
            }
        }
        return result;
    }

    std::uint8_t InputDeviceState::getModifiers() const noexcept
    {
        std::uint8_t result = InputModifier_None;
        using Key = Platform::Keyboard::Key;
        for (const auto button : _heldButtons)
        {
            if (button.isMouse())
            {
                continue;
            }
            switch (NormalizeInputButton(button).getKeyboardKey())
            {
                case Key::Left_Control:
                    result |= InputModifier_Control;
                    break;
                case Key::Left_Shift:
                    result |= InputModifier_Shift;
                    break;
                case Key::Left_Alt:
                    result |= InputModifier_Alt;
                    break;
                case Key::Left_Super:
                    result |= InputModifier_Super;
                    break;
                default:
                    break;
            }
        }
        return result;
    }

    void InputDeviceState::reset()
    {
        _heldButtons.clear();
        _pointerPosition.reset();
    }

    const std::vector<InputButton>& InputDeviceState::getHeldButtons() const noexcept
    {
        return _heldButtons;
    }
} // namespace NX
