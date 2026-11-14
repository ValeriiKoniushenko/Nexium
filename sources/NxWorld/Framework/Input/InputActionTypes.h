// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

// Module: Nexium::World

#pragma once

#include "Core/String.h"
#include "JustReflectMe/Adapter.h"
#include "NxFundamental/ResourceManagement/JsonAdapter.h"
#include "Platform/Input/Events/InputEvent.h"
#include "Platform/Input/InputDevices/Joystick.h"
#include "Platform/Input/InputDevices/Keyboard.h"
#include "Platform/Input/InputDevices/Mouse.h"
#include "glm/vec2.hpp"

#include <cstdint>
#include <string>
#include <vector>

namespace NX
{
    ENUM_CLASS();
    enum class InputSource : std::uint8_t
    {
        Key,
        MouseButton,
        MouseDelta,
        MouseScroll,
        GamepadButton,
        GamepadAxis
    };

    ENUM_CLASS();
    enum class InputTrigger : std::uint8_t
    {
        Down,
        Pressed,
        Released
    };

    /// How the held modifiers (Shift/Ctrl/Alt/Super) are compared with the binding's ones.
    /// Caps Lock and Num Lock are never taken into account.
    ENUM_CLASS();
    enum class ModifierMatch : std::uint8_t
    {
        /// The binding's modifiers must be held; extra ones are fine.
        /// Gameplay: W keeps moving forward while Shift is held for sprint.
        AtLeast,
        /// Exactly the binding's modifiers must be held.
        /// Shortcuts: Ctrl+S does not fire on Ctrl+Shift+S, Delete not on Shift+Delete.
        Exact
    };

    /// What is happening with one binding right now. Not reflected, so it is never saved.
    struct InputBindingRuntime
    {
        /// The button is down right now.
        bool isHeld = false;
        /// A press arrived this frame, even if the button was released again in the same frame.
        bool wasPressedThisFrame = false;
        /// The binding's Pressed/Released trigger fired this frame.
        bool firedThisFrame = false;
        /// Mouse delta/scroll summed over this frame, or the last normalized axis value.
        glm::vec2 value{ 0.f };
    };

    /// The combined result of all bindings of one action. Not reflected, so it is never saved.
    struct InputActionRuntime
    {
        glm::vec2 value{ 0.f };
        bool isActive = false;
        bool wasTriggered = false;
    };

    CLASS();
    struct InputBinding
    {
        R_FRIEND(InputBinding);

        FIELD();
        NX::InputSource source = NX::InputSource::Key;
        FIELD();
        std::uint16_t code = 0;
        FIELD();
        NX::InputTrigger trigger = NX::InputTrigger::Down;
        FIELD();
        bool shift = false;
        FIELD();
        bool control = false;
        FIELD();
        bool alt = false;
        FIELD();
        bool super = false;
        FIELD();
        NX::ModifierMatch modifierMatch = NX::ModifierMatch::AtLeast;
        FIELD();
        bool negate = false;
        FIELD();
        bool swizzleYX = false;
        /// Gamepad axes only: a normalized value with a smaller magnitude counts as zero.
        /// It is a threshold, not a rescale. 0 keeps every value.
        FIELD();
        float deadzone = 0.f;

        [[nodiscard]] static InputBinding Key(Platform::Keyboard::Key key) noexcept;
        /// Editor-style shortcut: exact modifiers, fires once on press.
        [[nodiscard]] static InputBinding Shortcut(Platform::Keyboard::Key key,
                                                   InputModifier modifiers
                                                   = InputModifier::None) noexcept;
        [[nodiscard]] static InputBinding MouseButton(Platform::Mouse::Key button) noexcept;
        [[nodiscard]] static InputBinding MouseDelta() noexcept;
        [[nodiscard]] static InputBinding MouseScroll() noexcept;
        [[nodiscard]] static InputBinding GamepadButton(
            Platform::Joystick::GamepadButton button) noexcept;
        [[nodiscard]] static InputBinding GamepadAxis(
            Platform::Joystick::GamepadAxis axis) noexcept;

        [[nodiscard]] InputBinding withShift() const noexcept;
        [[nodiscard]] InputBinding withControl() const noexcept;
        [[nodiscard]] InputBinding withAlt() const noexcept;
        [[nodiscard]] InputBinding withSuper() const noexcept;
        [[nodiscard]] InputBinding withExactModifiers() const noexcept;
        [[nodiscard]] InputBinding onDown() const noexcept;
        [[nodiscard]] InputBinding onPressed() const noexcept;
        [[nodiscard]] InputBinding onReleased() const noexcept;
        [[nodiscard]] InputBinding negated() const noexcept;
        [[nodiscard]] InputBinding swizzled() const noexcept;
        [[nodiscard]] InputBinding withDeadzone(float value) const noexcept;

        [[nodiscard]] InputModifier getModifiers() const noexcept;
        [[nodiscard]] bool isButton() const noexcept;
        [[nodiscard]] bool isPerFrame() const noexcept;
        [[nodiscard]] bool matches(InputSource eventSource, std::uint16_t eventCode) const noexcept;
        [[nodiscard]] bool acceptsModifiers(InputModifier mods) const noexcept;
        [[nodiscard]] glm::vec2 applyValueModifiers(glm::vec2 value) const noexcept;
        /// Human-readable form for UI, e.g. "Ctrl+Shift+S", "Mouse Left", "Gamepad LeftX".
        [[nodiscard]] std::string toString() const;

        // Runtime, driven by InputController once per frame.
        void beginFrame() noexcept;
        void handleButton(InputState state, InputModifier mods) noexcept;
        void handleVector(glm::vec2 delta) noexcept;
        void handleAxis(float normalizedValue) noexcept;
        /// Lets go of a held button and returns an axis to rest, e.g. when the window loses focus.
        /// Edges and values already collected this frame are kept.
        void releaseHeld() noexcept;
        /// This binding's share of the action value for the current frame.
        [[nodiscard]] glm::vec2 getContribution() const noexcept;

        NX::InputBindingRuntime runtime;
    };

    void to_json(nlohmann::json& j, const InputBinding& v);
    void from_json(const nlohmann::json& j, InputBinding& v);

    CLASS();
    struct InputAction
    {
        R_FRIEND(InputAction);

        InputAction() = default;
        InputAction(Core::StringAtom name, std::vector<InputBinding> bindings);

        FIELD();
        Core::StringAtom name;
        FIELD();
        std::vector<NX::InputBinding> bindings;

        // Runtime, driven by InputController once per frame.
        void beginFrame() noexcept;
        /// Combines the bindings' contributions into `runtime`.
        void endFrame() noexcept;
        /// Releases every held button and axis of the action without touching this frame's edges.
        void releaseHeld() noexcept;
        /// Forgets held buttons and values of the action and all its bindings.
        void resetRuntime() noexcept;

        NX::InputActionRuntime runtime;
    };

    void to_json(nlohmann::json& j, const InputAction& v);
    void from_json(const nlohmann::json& j, InputAction& v);
} // namespace NX

#include "InputActionTypes.generated.h" // added by the code generator. Better don't move it.
