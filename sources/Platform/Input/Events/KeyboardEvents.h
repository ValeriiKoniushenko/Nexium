// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

// Module: Nexium::Subsystem::Input

#pragma once

#include "InputEvent.h"

namespace NX
{
    class KeyboardEvent : public InputEvent
    {
    public:
        KeyboardEvent()
            : InputEvent(EventType::Keyboard, InputDeviceType::Keyboard)
        {
        }

        void setKey(InputButton key) { _key = key; }
        void setState(InputState state) { _state = state; }
        void setScancode(int scancode) { _scancode = scancode; }
        void setMods(InputModifier mods) { _mods = mods; }

        [[nodiscard]] InputButton getKey() const { return _key; }
        [[nodiscard]] InputState getState() const { return _state; }
        [[nodiscard]] int getScancode() const { return _scancode; }
        [[nodiscard]] InputModifier getMods() const { return _mods; }

    private:
        InputButton _key = InputButton::InvalidIndex;
        InputState _state = InputState::None;
        int _scancode = 0;
        InputModifier _mods = InputModifier::None;
    };

    class TextInputEvent : public InputEvent
    {
    public:
        TextInputEvent()
            : InputEvent(EventType::TextInput, InputDeviceType::Keyboard)
        {
        }

        void setCodepoint(uint32_t codepoint) { _codepoint = codepoint; }

        [[nodiscard]] uint32_t getCodepoint() const { return _codepoint; }

    private:
        uint32_t _codepoint = 0;
    };

} // namespace NX