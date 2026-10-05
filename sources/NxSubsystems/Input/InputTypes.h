// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "Core/String.h"
#include "Platform/Keyboard.h"
#include "Platform/Mouse.h"

#include <cstddef>
#include <cstdint>
#include <variant>
#include <vector>

namespace NX
{
    ENUM_CLASS();
    enum class InputContext : std::uint8_t
    {
        Editor,
        Gameplay
    };

    ENUM_CLASS();
    enum class InputModifier : std::uint8_t
    {
        None = 0,
        Shift = 1 << 0,
        Control = 1 << 1,
        Alt = 1 << 2,
        Super = 1 << 3,
        All = (1 << 4) - 1
    };

    class InputButton
    {
    public:
        constexpr InputButton() = default;
        constexpr InputButton(Platform::Keyboard::Key key)
            : _value(key)
        {
        }
        constexpr InputButton(Platform::Mouse::Key button)
            : _value(button)
        {
        }

        [[nodiscard]] constexpr bool isMouse() const noexcept
        {
            return std::holds_alternative<Platform::Mouse::Key>(_value);
        }

        [[nodiscard]] constexpr Platform::Keyboard::Key getKeyboardKey() const noexcept
        {
            if (const auto* key = std::get_if<Platform::Keyboard::Key>(&_value))
            {
                return *key;
            }
            return Platform::Keyboard::Key::None;
        }

        [[nodiscard]] constexpr Platform::Mouse::Key getMouseButton() const noexcept
        {
            if (const auto* button = std::get_if<Platform::Mouse::Key>(&_value))
            {
                return *button;
            }
            return Platform::Mouse::Key::None;
        }

        [[nodiscard]] constexpr bool isNone() const noexcept
        {
            return isMouse() ? getMouseButton() == Platform::Mouse::Key::None
                             : getKeyboardKey() == Platform::Keyboard::Key::None;
        }

        [[nodiscard]] constexpr bool operator==(const InputButton&) const noexcept = default;

    private:
        std::variant<Platform::Keyboard::Key, Platform::Mouse::Key> _value
            = Platform::Keyboard::Key::None;
    };

    /// @brief RAII guard that suspends input actions while recording a shortcut.
    /// All instances share capture state: actions remain suspended while any guard exists.
    /// After the last guard is destroyed, suspension continues until all tracked held
    /// keyboard keys and mouse buttons are released or resetButtons() clears them.
    class InputCapture
    {
    public:
        InputCapture();
        ~InputCapture();

        InputCapture(const InputCapture&) = delete;
        InputCapture(InputCapture&&) = delete;
        InputCapture& operator=(const InputCapture&) = delete;
        InputCapture& operator=(InputCapture&&) = delete;

        /// Returns whether a guard exists or held buttons are still awaiting release.
        [[nodiscard]] static bool isActive() noexcept;

        /// Tracks button presses and releases, even when no guard exists.
        static void updateButton(InputButton button, Platform::Keyboard::KeyState state);

        /// Clears button tracking and pending releases without ending live captures.
        static void resetButtons();

    private:
        static std::size_t _captureCount;
        static bool _awaitingRelease;
        static std::vector<InputButton> _heldButtons;
    };

    struct KeyChord
    {
        /// Keyboard key or mouse button that completes the chord and triggers its action.
        InputButton triggerKey;
        /// Keys and mouse buttons that must already be held when triggerKey is pressed.
        std::vector<InputButton> requiredKeys;

        [[nodiscard]] static KeyChord Exact(InputButton key);

        [[nodiscard]] bool matches(InputButton eventKey,
                                   const std::vector<InputButton>& pressedKeys) const;

        [[nodiscard]] bool contains(InputButton key) const;
    };

    ENUM_CLASS();
    enum class InputActionTrigger : std::uint8_t
    {
        WhileHeld,
        OnPress,
        OnRelease
    };

    struct KeyInputEvent
    {
        InputButton key;
        Platform::Keyboard::KeyState state = Platform::Keyboard::KeyState::None;
        InputModifier modifiers = InputModifier::None;
        int scancode = 0;
        std::vector<InputButton> pressedKeys;
    };

    struct InputActionEvent
    {
        Core::StringAtom action;
        Platform::Keyboard::KeyState state = Platform::Keyboard::KeyState::None;

        [[nodiscard]] constexpr bool isPressed() const noexcept
        {
            return state == Platform::Keyboard::KeyState::Pressed;
        }

        [[nodiscard]] constexpr bool isReleased() const noexcept
        {
            return state == Platform::Keyboard::KeyState::Released;
        }

        [[nodiscard]] constexpr bool isRepeated() const noexcept
        {
            return state == Platform::Keyboard::KeyState::Repeated;
        }
    };
} // namespace NX

// Keep bitwise operators in the global namespace, consistently with Core::Tag operators.
// Otherwise a Core::operator| overload hides ::operator|(Core::Tag, Core::Tag) in Core code.
[[nodiscard]] constexpr NX::InputModifier operator|(NX::InputModifier lhs, NX::InputModifier rhs)
{
    return static_cast<NX::InputModifier>(static_cast<std::uint8_t>(lhs)
                                          | static_cast<std::uint8_t>(rhs));
}

[[nodiscard]] constexpr NX::InputModifier operator&(NX::InputModifier lhs, NX::InputModifier rhs)
{
    return static_cast<NX::InputModifier>(static_cast<std::uint8_t>(lhs)
                                          & static_cast<std::uint8_t>(rhs));
}

#include "InputTypes.generated.h" // added by the code generator. Better don't move it.
