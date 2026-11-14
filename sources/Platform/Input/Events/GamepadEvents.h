// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

// Module: Nexium::Platform::Input

#pragma once

#include "InputEvent.h"

namespace NX
{

    class GamepadEvent : public InputEvent
    {
    protected:
        explicit GamepadEvent(EventType eventType, InputDeviceId deviceId = InvalidInputDeviceId)
            : InputEvent(eventType, InputDeviceType::Gamepad, deviceId)
        {
        }
    };

    class GamepadButtonEvent : public GamepadEvent
    {
    public:
        explicit GamepadButtonEvent(InputDeviceId deviceId = InvalidInputDeviceId)
            : GamepadEvent(EventType::GamepadButton, deviceId)
        {
        }

        void setButton(InputButton button) { _button = button; }
        void setState(InputState state) { _state = state; }

        [[nodiscard]] InputButton getButton() const { return _button; }
        [[nodiscard]] InputState getState() const { return _state; }

    private:
        InputState _state = InputState::None;
        InputButton _button = InputButton::InvalidIndex;
    };

    class GamepadAxisEvent : public GamepadEvent
    {
    public:
        explicit GamepadAxisEvent(InputDeviceId deviceId = InvalidInputDeviceId)
            : GamepadEvent(EventType::GamepadAxis, deviceId)
        {
        }

        void setAxis(std::uint16_t axis) { _axis = axis; }
        void setValue(float value) { _value = value; }

        [[nodiscard]] std::uint16_t getAxis() const { return _axis; }
        [[nodiscard]] float getValue() const { return _value; }

    private:
        std::uint16_t _axis = 0;
        float _value = 0.f;
    };

    class GamepadConnectionEvent : public GamepadEvent
    {
    public:
        explicit GamepadConnectionEvent(InputDeviceId deviceId = InvalidInputDeviceId)
            : GamepadEvent(EventType::GamepadConnection, deviceId)
        {
        }

        void setConnected(bool connected) { _connected = connected; }

        [[nodiscard]] bool isConnected() const { return _connected; }

    private:
        bool _connected = false;
    };

} // namespace NX
