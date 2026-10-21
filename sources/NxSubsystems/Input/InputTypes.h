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
#include "glm/vec2.hpp"

#include <cstddef>
#include <cstdint>
#include <variant>
#include <vector>

namespace NX
{
    // tag::input-enums[]
    ENUM_CLASS();
    enum class InputContext : std::uint8_t
    {
        Editor,
        Gameplay,
        EditorViewport,
        Modal,
        TextInput,
        BindingCapture,
    };

    // Bit masks: plain enums, so the flags combine with the built-in bitwise operators.
    enum InputChannel : std::uint8_t
    {
        InputChannel_Keyboard = 1 << 0,
        InputChannel_Pointer = 1 << 1, // Mouse
        InputChannel_All = InputChannel_Keyboard | InputChannel_Pointer,
    };

    enum class InputEventType : std::uint8_t
    {
        Button,
        PointerMove,
        Wheel,
    };

    enum InputModifier : std::uint8_t
    {
        InputModifier_None = 0,
        InputModifier_Shift = 1 << 0,
        InputModifier_Control = 1 << 1,
        InputModifier_Alt = 1 << 2,
        InputModifier_Super = 1 << 3,
        InputModifier_All = (1 << 4) - 1,
    };
    // end::input-enums[]

    // tag::input-button-api[]
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

    [[nodiscard]] InputButton NormalizeInputButton(InputButton button) noexcept;
    [[nodiscard]] bool IsInputModifier(InputButton button) noexcept;
    // end::input-button-api[]

    /// @brief RAII guard that suspends input actions while recording a shortcut.
    /// All instances share capture state: actions remain suspended while any guard exists.
    /// After the last guard is destroyed, suspension continues until all tracked held
    /// keyboard keys and mouse buttons are released or resetButtons() clears them.
    // tag::input-capture-api[]
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
        // end::input-capture-api[]

    private:
        static std::size_t _captureCount;
        static bool _awaitingRelease;
        static std::vector<InputButton> _heldButtons;
    };

    // tag::input-chord[]
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
    // end::input-chord[]

    // tag::input-trigger[]
    ENUM_CLASS();
    enum class InputActionTrigger : std::uint8_t
    {
        WhileHeld,
        OnPress,
        OnRelease
    };
    // end::input-trigger[]

    // tag::input-events[]
    struct KeyInputEvent
    {
        InputButton key;
        Platform::Keyboard::KeyState state = Platform::Keyboard::KeyState::None;
        std::uint8_t modifiers = InputModifier_None;
        int scancode = 0;
        std::vector<InputButton> pressedKeys;
        InputEventType type = InputEventType::Button;
        glm::vec2 value{};
        glm::vec2 position{};

        [[nodiscard]] bool isButtonEventType() const { return type == InputEventType::Button; }
        [[nodiscard]] bool isPointerMoveEventType() const
        {
            return type == InputEventType::PointerMove;
        }

        [[nodiscard]] bool isKeyStatePressed() const
        {
            return state == Platform::Keyboard::KeyState::Pressed;
        }
    };

    struct InputActionEvent
    {
        Core::StringAtom action;
        Platform::Keyboard::KeyState state = Platform::Keyboard::KeyState::None;
        glm::vec2 value{};
        glm::vec2 position{};
        bool canceled = false;
        std::uint8_t modifiers = InputModifier_None;

        [[nodiscard]] constexpr bool isCanceled() const noexcept { return canceled; }

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
    // end::input-events[]
} // namespace NX

#include "InputTypes.generated.h" // added by the code generator. Better don't move it.
