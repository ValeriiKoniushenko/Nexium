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
#include "glm/glm.hpp"

namespace NX
{

    class MouseEvent : public InputEvent
    {
    protected:
        explicit MouseEvent(EventType eventType, InputDeviceId deviceId = InvalidInputDeviceId)
            : InputEvent(eventType, InputDeviceType::Mouse, deviceId)
        {
        }
    };

    class MouseMoveEvent : public MouseEvent
    {
    public:
        explicit MouseMoveEvent(InputDeviceId deviceId = InvalidInputDeviceId)
            : MouseEvent(EventType::MouseMove, deviceId)
        {
        }

        void setPosition(const glm::vec2& position) { _position = position; }

        void setDelta(const glm::vec2& delta) { _delta = delta; }

        [[nodiscard]] const glm::vec2& getPosition() const { return _position; }

        [[nodiscard]] const glm::vec2& getDelta() const { return _delta; }

    private:
        glm::vec2 _position{ 0.f };
        glm::vec2 _delta{ 0.f };
    };

    class MouseButtonEvent : public MouseEvent
    {
    public:
        explicit MouseButtonEvent(InputDeviceId deviceId = InvalidInputDeviceId)
            : MouseEvent(EventType::MouseButton, deviceId)
        {
        }

        void setButton(InputButton button) { _button = button; }
        void setState(InputState state) { _state = state; }
        void setMods(InputModifier mods) { _mods = mods; }
        void setPosition(const glm::vec2& position) { _position = position; }

        [[nodiscard]] InputButton getButton() const { return _button; }
        [[nodiscard]] InputState getState() const { return _state; }
        [[nodiscard]] InputModifier getMods() const { return _mods; }

        /// Cursor position (window coordinates) at the moment the button changed state. Use it
        /// instead of the current cursor position: the mouse may have moved since the click.
        [[nodiscard]] const glm::vec2& getPosition() const { return _position; }

    private:
        InputState _state = InputState::None;
        InputModifier _mods = InputModifier::None;
        InputButton _button = InputButton::InvalidIndex;
        glm::vec2 _position{ 0.f };
    };

    class MouseScrollEvent : public MouseEvent
    {
    public:
        MouseScrollEvent()
            : MouseEvent(EventType::MouseScroll)
        {
        }

        void setOffset(const glm::vec2& offset) { _offset = offset; }

        [[nodiscard]] const glm::vec2& getOffset() const { return _offset; }

    private:
        glm::vec2 _offset{ 0.f };
    };

    class MouseEnterEvent : public MouseEvent
    {
    public:
        MouseEnterEvent()
            : MouseEvent(EventType::MouseEnter)
        {
        }

        void setEntered(bool entered) { _entered = entered; }

        [[nodiscard]] bool isEntered() const { return _entered; }

    private:
        bool _entered = false;
    };

} // namespace NX