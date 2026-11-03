// // Nexium
// // Copyright 2018-2026 Valerii Koniushenko
// //
// // Licensed under the Apache License, Version 2.0 (the "License");
// // you may not use this file except in compliance with the License.
// // You may obtain a copy of the License at
// //
// //     http://www.apache.org/licenses/LICENSE-2.0

// // Module: Nexium::Subsystem::Input

// #pragma once

// #include "Core/String.h"
// #include "Platform/Input/InputDevices/

// #include <cstdint>
// #include <vector>

// namespace NX
// {

//     // ENUM_CLASS();
//     enum class InputState;
//     // {
//     //     None = 0,
//     //     // mouse event states
//     //     Move,
//     //     Wheel,
//     //     // any phisical buttons
//     //     Pressed = GLFW_PRESS,
//     //     Released = GLFW_RELEASE,
//     //     Repeated = GLFW_REPEAT
//     // };

//     // class InputButton
//     // {
//     // public:
//     //     InputButton() = default;
//     //     constexpr InputButton(std::uint16_t index)
//     //         : _index(index)
//     //     {
//     //     }

//     //     void reset() { _index = 0; }

//     //     [[nodiscard]] constexpr std::uint16_t getIndex() const { return _index; }
//     //     [[nodiscard]] constexpr bool operator==(const InputButton&) const = default;

//     // private:
//     //     std::uint16_t _index;
//     // };

//     // ENUM_CLASS();
//     enum class InputModifier : std::uint8_t;
//     // {
//     //     None = 0,
//     //     Shift = 1 << 0,
//     //     Control = 1 << 1,
//     //     Alt = 1 << 2,
//     //     Super = 1 << 3,
//     //     CapsLock = 1 << 4,
//     //     NumLock = 1 << 5,
//     //     All = (1 << 6) - 1
//     // };

//     struct InputEvent;
//     // {
//     //     InputButton button;
//     //     InputState state;
//     //     InputModifier mods;
//     //     int scancode;
//     //     std::vector<InputButton> allPressedButtons;
//     // };

//     //////////////////////////////

//     ENUM_CLASS();
//     enum class InputContext : std::uint8_t
//     {
//         Editor,
//         Gameplay
//     };

//     struct KeyChord
//     {
//         // /// Key that completes the chord and triggers its action (for example S in
//         Ctrl+Shift+S).
//         // NX::InputButton triggerKey;
//         // /// Keys that must already be held when triggerKey is pressed (for example Ctrl and
//         // Shift). std::vector<NX::InputButton> requiredKeys;

//         // [[nodiscard]] static KeyChord Exact(NX::InputButton key);

//         // [[nodiscard]] bool matches(NX::InputButton eventKey,
//         //                            const std::vector<NX::InputButton>& pressedKeys) const;

//         // [[nodiscard]] bool contains(NX::InputButton key) const;
//     };

//     struct InputActionEvent
//     {
//         // Core::StringAtom action;
//         // InputState state = InputState::None;

//         // [[nodiscard]] constexpr bool isPressed() const noexcept
//         // {
//         //     return state == InputState::Pressed;
//         // }

//         // [[nodiscard]] constexpr bool isReleased() const noexcept
//         // {
//         //     return state == InputState::Released;
//         // }

//         // [[nodiscard]] constexpr bool isRepeated() const noexcept
//         // {
//         //     return state == InputState::Repeated;
//         // }
//     };
// } // namespace NX

// // // Keep bitwise operators in the global namespace, consistently with Core::Tag operators.
// // // Otherwise a Core::operator| overload hides ::operator|(Core::Tag, Core::Tag) in Core code.
// // [[nodiscard]] constexpr NX::InputModifier operator|(NX::InputModifier lhs, NX::InputModifier
// rhs)
// // {
// //     return static_cast<NX::InputModifier>(static_cast<std::uint8_t>(lhs)
// //                                           | static_cast<std::uint8_t>(rhs));
// // }

// // [[nodiscard]] constexpr NX::InputModifier operator&(NX::InputModifier lhs, NX::InputModifier
// rhs)
// // {
// //     return static_cast<NX::InputModifier>(static_cast<std::uint8_t>(lhs)
// //                                           & static_cast<std::uint8_t>(rhs));
// // }

// #include "InputTypes.generated.h" // added by the code generator. Better don't move it.
