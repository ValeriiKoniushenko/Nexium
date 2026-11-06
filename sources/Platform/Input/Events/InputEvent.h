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

#include "Core/String.h"
#include "JustReflectMe/Adapter.h"

namespace NX
{

    /// Platform-independent button state. Backends convert their native values explicitly
    /// (see Platform::ToInputState for GLFW); never cast native constants to this enum.
    ENUM_CLASS();
    enum class InputState : uint8_t
    {
        None = 0,
        Pressed,
        Released,
        Repeated
    };

    ENUM_CLASS();
    enum class InputModifier : std::uint8_t
    {
        None = 0,
        Shift = 1 << 0,
        Control = 1 << 1,
        Alt = 1 << 2,
        Super = 1 << 3,
        CapsLock = 1 << 4,
        NumLock = 1 << 5,
        All = (1 << 6) - 1
    };

    constexpr InputModifier operator|(InputModifier lhs, InputModifier rhs)
    {
        return static_cast<InputModifier>(static_cast<std::uint8_t>(lhs)
                                          | static_cast<std::uint8_t>(rhs));
    }

    constexpr InputModifier operator&(InputModifier lhs, InputModifier rhs)
    {
        return static_cast<InputModifier>(static_cast<std::uint8_t>(lhs)
                                          & static_cast<std::uint8_t>(rhs));
    }

    enum class EventType : uint8_t
    {
        None,

        Keyboard,

        MouseMove,
        MouseEnter,
        MouseButton,
        MouseScroll,

        GamepadAxis,
        GamepadButton,
        GamepadConnection,

        TouchEnd,
        TouchMove,
        TouchBegin,

        WindowResize,
        WindowFocus,
        WindowIconify,
        WindowClose,

        TextInput,
    };

    enum class InputDeviceType : uint8_t
    {
        None,
        Touch,
        Mouse,
        Gamepad,
        Keyboard,
    };

    using InputDeviceId = uint32_t;
    constexpr InputDeviceId InvalidInputDeviceId = 0;

    /// Common data of every input event. Events are small value types stored by value (see
    /// AnyInputEvent), so this base is intentionally non-polymorphic: there is no virtual
    /// destructor, and copying/destruction through a base reference is not allowed.
    class InputEvent
    {
    public:
        [[nodiscard]] EventType getEventType() const { return _eventType; }

        [[nodiscard]] InputDeviceType getDeviceType() const { return _deviceType; }

        [[nodiscard]] InputDeviceId getDeviceId() const { return _deviceId; }

    protected:
        ~InputEvent() = default;
        InputEvent(const InputEvent&) = default;
        InputEvent(InputEvent&&) = default;
        InputEvent& operator=(const InputEvent&) = default;
        InputEvent& operator=(InputEvent&&) = default;

        explicit InputEvent(EventType eventType, InputDeviceType deviceType,
                            InputDeviceId deviceId = InvalidInputDeviceId)
            : _eventType(eventType),
              _deviceType(deviceType),
              _deviceId(deviceId)
        {
        }

    private:
        EventType _eventType = EventType::None;
        InputDeviceType _deviceType = InputDeviceType::None;
        InputDeviceId _deviceId = InvalidInputDeviceId;
    };

    class InputButton
    {
    public:
        static constexpr std::uint16_t InvalidIndex = UINT16_MAX;

        InputButton() = default;

        constexpr InputButton(std::uint16_t index)
            : _index(index)
        {
        }

        void reset() { _index = InvalidIndex; }

        [[nodiscard]] constexpr bool isValid() const { return _index != InvalidIndex; }

        [[nodiscard]] constexpr std::uint16_t getIndex() const { return _index; }

        [[nodiscard]] constexpr bool operator==(const InputButton&) const = default;

    private:
        std::uint16_t _index = InvalidIndex;
    };

} // namespace NX

#include "InputEvent.generated.h" // added by the code generator. Better don't move it.