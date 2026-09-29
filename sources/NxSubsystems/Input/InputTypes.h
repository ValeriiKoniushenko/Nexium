/*
 * MIT License
 *
 * Copyright (c) 2018-2027 Valerii Koniushenko
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#pragma once

#include "Core/String.h"
#include "Platform/Keyboard.h"

#include <cstdint>
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

    struct KeyChord
    {
        /// Key that completes the chord and triggers its action (for example S in Ctrl+Shift+S).
        Platform::Keyboard::Key triggerKey = Platform::Keyboard::Key::None;
        /// Keys that must already be held when triggerKey is pressed (for example Ctrl and Shift).
        std::vector<Platform::Keyboard::Key> requiredKeys;

        [[nodiscard]] static KeyChord Exact(Platform::Keyboard::Key key);

        [[nodiscard]] bool matches(Platform::Keyboard::Key eventKey,
                                   const std::vector<Platform::Keyboard::Key>& pressedKeys) const;

        [[nodiscard]] bool contains(Platform::Keyboard::Key key) const;
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
        Platform::Keyboard::Key key = Platform::Keyboard::Key::None;
        Platform::Keyboard::KeyState state = Platform::Keyboard::KeyState::None;
        InputModifier modifiers = InputModifier::None;
        int scancode = 0;
        std::vector<Platform::Keyboard::Key> pressedKeys;
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
