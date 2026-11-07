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
        bool negate = false;
        FIELD();
        bool swizzleYX = false;

        [[nodiscard]] static InputBinding Key(Platform::Keyboard::Key key) noexcept;
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
        [[nodiscard]] InputBinding onPressed() const noexcept;
        [[nodiscard]] InputBinding onReleased() const noexcept;
        [[nodiscard]] InputBinding negated() const noexcept;
        [[nodiscard]] InputBinding swizzled() const noexcept;

        [[nodiscard]] InputModifier getModifiers() const noexcept;
        [[nodiscard]] bool isButton() const noexcept;
        [[nodiscard]] bool isPerFrame() const noexcept;
        [[nodiscard]] bool matches(InputSource eventSource, std::uint16_t eventCode) const noexcept;
        [[nodiscard]] bool acceptsModifiers(InputModifier mods) const noexcept;
        [[nodiscard]] glm::vec2 applyValueModifiers(glm::vec2 value) const noexcept;

        // Runtime, driven by InputController once per frame.
        void beginFrame() noexcept;
        void handleButton(InputState state, InputModifier mods) noexcept;
        void handleVector(glm::vec2 delta) noexcept;
        void handleAxis(float normalizedValue) noexcept;
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
        /// Forgets held buttons and values of the action and all its bindings.
        void resetRuntime() noexcept;

        NX::InputActionRuntime runtime;
    };

    void to_json(nlohmann::json& j, const InputAction& v);
    void from_json(const nlohmann::json& j, InputAction& v);
} // namespace NX

#include "InputActionTypes.generated.h" // added by the code generator. Better don't move it.
