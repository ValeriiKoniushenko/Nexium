// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "NxWorld/Framework/Input/InputActionTypes.h"

#include <string_view>

namespace
{
    constexpr auto MatchedModifiers = NX::InputModifier::Shift | NX::InputModifier::Control
                                      | NX::InputModifier::Alt | NX::InputModifier::Super;
} // namespace

namespace
{
    NX::InputBinding MakeBinding(NX::InputSource source, std::uint16_t code = 0) noexcept
    {
        NX::InputBinding binding;
        binding.source = source;
        binding.code = code;
        return binding;
    }

    std::string_view GamepadButtonName(std::uint16_t code) noexcept
    {
        using Button = Platform::Joystick::GamepadButton;
        switch (static_cast<Button>(code))
        {
            case Button::A:
                return "A";
            case Button::B:
                return "B";
            case Button::X:
                return "X";
            case Button::Y:
                return "Y";
            case Button::LeftBumper:
                return "LeftBumper";
            case Button::RightBumper:
                return "RightBumper";
            case Button::Back:
                return "Back";
            case Button::Start:
                return "Start";
            case Button::Guide:
                return "Guide";
            case Button::LeftThumb:
                return "LeftThumb";
            case Button::RightThumb:
                return "RightThumb";
            case Button::DpadUp:
                return "DpadUp";
            case Button::DpadRight:
                return "DpadRight";
            case Button::DpadDown:
                return "DpadDown";
            case Button::DpadLeft:
                return "DpadLeft";
            default:
                return {};
        }
    }

    std::string_view GamepadAxisName(std::uint16_t code) noexcept
    {
        using Axis = Platform::Joystick::GamepadAxis;
        switch (static_cast<Axis>(code))
        {
            case Axis::LeftX:
                return "LeftX";
            case Axis::LeftY:
                return "LeftY";
            case Axis::RightX:
                return "RightX";
            case Axis::RightY:
                return "RightY";
            case Axis::LeftTrigger:
                return "LeftTrigger";
            case Axis::RightTrigger:
                return "RightTrigger";
            default:
                return {};
        }
    }

    std::string MouseButtonName(std::uint16_t code)
    {
        // Left/Right/Middle alias _1/_2/_3, so they are named here instead of via reflection.
        switch (static_cast<Platform::Mouse::Key>(code))
        {
            case Platform::Mouse::Key::Left:
                return "Left";
            case Platform::Mouse::Key::Right:
                return "Right";
            case Platform::Mouse::Key::Middle:
                return "Middle";
            default:
                return std::to_string(code + 1);
        }
    }

    std::string NameOrCode(std::string_view name, std::uint16_t code)
    {
        return name.empty() ? "#" + std::to_string(code) : std::string(name);
    }
} // namespace

namespace NX
{
    InputBinding InputBinding::Key(Platform::Keyboard::Key key) noexcept
    {
        return MakeBinding(InputSource::Key, static_cast<std::uint16_t>(key));
    }

    InputBinding InputBinding::Shortcut(Platform::Keyboard::Key key,
                                        InputModifier modifiers) noexcept
    {
        auto binding = Key(key);
        binding.shift = (modifiers & InputModifier::Shift) != InputModifier::None;
        binding.control = (modifiers & InputModifier::Control) != InputModifier::None;
        binding.alt = (modifiers & InputModifier::Alt) != InputModifier::None;
        binding.super = (modifiers & InputModifier::Super) != InputModifier::None;
        binding.trigger = InputTrigger::Pressed;
        binding.modifierMatch = ModifierMatch::Exact;
        return binding;
    }

    InputBinding InputBinding::MouseButton(Platform::Mouse::Key button) noexcept
    {
        return MakeBinding(InputSource::MouseButton, static_cast<std::uint16_t>(button));
    }

    InputBinding InputBinding::MouseDelta() noexcept
    {
        return MakeBinding(InputSource::MouseDelta);
    }

    InputBinding InputBinding::MouseScroll() noexcept
    {
        return MakeBinding(InputSource::MouseScroll);
    }

    InputBinding InputBinding::GamepadButton(Platform::Joystick::GamepadButton button) noexcept
    {
        return MakeBinding(InputSource::GamepadButton, static_cast<std::uint16_t>(button));
    }

    InputBinding InputBinding::GamepadAxis(Platform::Joystick::GamepadAxis axis) noexcept
    {
        return MakeBinding(InputSource::GamepadAxis, static_cast<std::uint16_t>(axis));
    }

    InputBinding InputBinding::withShift() const noexcept
    {
        auto copy = *this;
        copy.shift = true;
        return copy;
    }

    InputBinding InputBinding::withControl() const noexcept
    {
        auto copy = *this;
        copy.control = true;
        return copy;
    }

    InputBinding InputBinding::withAlt() const noexcept
    {
        auto copy = *this;
        copy.alt = true;
        return copy;
    }

    InputBinding InputBinding::withSuper() const noexcept
    {
        auto copy = *this;
        copy.super = true;
        return copy;
    }

    InputBinding InputBinding::withExactModifiers() const noexcept
    {
        auto copy = *this;
        copy.modifierMatch = ModifierMatch::Exact;
        return copy;
    }

    InputBinding InputBinding::onPressed() const noexcept
    {
        auto copy = *this;
        copy.trigger = InputTrigger::Pressed;
        return copy;
    }

    InputBinding InputBinding::onDown() const noexcept
    {
        auto copy = *this;
        copy.trigger = InputTrigger::Down;
        return copy;
    }

    InputBinding InputBinding::onReleased() const noexcept
    {
        auto copy = *this;
        copy.trigger = InputTrigger::Released;
        return copy;
    }

    InputBinding InputBinding::negated() const noexcept
    {
        auto copy = *this;
        copy.negate = true;
        return copy;
    }

    InputBinding InputBinding::swizzled() const noexcept
    {
        auto copy = *this;
        copy.swizzleYX = true;
        return copy;
    }

    InputModifier InputBinding::getModifiers() const noexcept
    {
        auto result = InputModifier::None;
        if (shift)
        {
            result = result | InputModifier::Shift;
        }
        if (control)
        {
            result = result | InputModifier::Control;
        }
        if (alt)
        {
            result = result | InputModifier::Alt;
        }
        if (super)
        {
            result = result | InputModifier::Super;
        }
        return result;
    }

    bool InputBinding::isButton() const noexcept
    {
        return source == InputSource::Key || source == InputSource::MouseButton
               || source == InputSource::GamepadButton;
    }

    bool InputBinding::isPerFrame() const noexcept
    {
        return source == InputSource::MouseDelta || source == InputSource::MouseScroll;
    }

    bool InputBinding::matches(InputSource eventSource, std::uint16_t eventCode) const noexcept
    {
        // Mouse delta and scroll have no button code, so only the source has to match.
        return source == eventSource && (isPerFrame() || code == eventCode);
    }

    bool InputBinding::acceptsModifiers(InputModifier mods) const noexcept
    {
        const auto required = getModifiers();
        const auto held = mods & MatchedModifiers;
        if (modifierMatch == ModifierMatch::Exact)
        {
            return held == required;
        }
        return (held & required) == required;
    }

    glm::vec2 InputBinding::applyValueModifiers(glm::vec2 value) const noexcept
    {
        if (negate)
        {
            value = -value;
        }
        if (swizzleYX)
        {
            value = glm::vec2{ value.y, value.x };
        }
        return value;
    }

    std::string InputBinding::toString() const
    {
        std::string result;
        if (control)
        {
            result += "Ctrl+";
        }
        if (shift)
        {
            result += "Shift+";
        }
        if (alt)
        {
            result += "Alt+";
        }
        if (super)
        {
            result += "Super+";
        }

        switch (source)
        {
            case InputSource::Key:
                result += NameOrCode(R<Platform::Keyboard::Key>::ToString(
                                         static_cast<Platform::Keyboard::Key>(code)),
                                     code);
                break;
            case InputSource::MouseButton:
                result += "Mouse " + MouseButtonName(code);
                break;
            case InputSource::MouseDelta:
                result += "Mouse Move";
                break;
            case InputSource::MouseScroll:
                result += "Mouse Wheel";
                break;
            case InputSource::GamepadButton:
                result += "Gamepad " + NameOrCode(GamepadButtonName(code), code);
                break;
            case InputSource::GamepadAxis:
                result += "Gamepad " + NameOrCode(GamepadAxisName(code), code);
                break;
        }

        if (trigger == InputTrigger::Released)
        {
            result += " (release)";
        }
        return result;
    }

    void InputBinding::beginFrame() noexcept
    {
        runtime.wasPressedThisFrame = false;
        runtime.firedThisFrame = false;
        if (isPerFrame())
        {
            runtime.value = glm::vec2{ 0.f };
        }
    }

    void InputBinding::handleButton(InputState state, InputModifier mods) noexcept
    {
        if (state == InputState::Pressed && acceptsModifiers(mods))
        {
            runtime.isHeld = true;
            runtime.wasPressedThisFrame = true;
            runtime.firedThisFrame |= trigger == InputTrigger::Pressed;
        }
        else if (state == InputState::Released && runtime.isHeld)
        {
            runtime.isHeld = false;
            runtime.firedThisFrame |= trigger == InputTrigger::Released;
        }
    }

    void InputBinding::handleVector(glm::vec2 delta) noexcept
    {
        runtime.value += delta;
    }

    void InputBinding::handleAxis(float normalizedValue) noexcept
    {
        runtime.value = glm::vec2{ normalizedValue, 0.f };
    }

    glm::vec2 InputBinding::getContribution() const noexcept
    {
        if (!isButton())
        {
            return applyValueModifiers(runtime.value);
        }

        const bool isOn = trigger == InputTrigger::Down ? runtime.isHeld : runtime.firedThisFrame;
        return applyValueModifiers(glm::vec2{ isOn ? 1.f : 0.f, 0.f });
    }

    InputAction::InputAction(Core::StringAtom name, std::vector<InputBinding> bindings)
        : name(std::move(name)),
          bindings(std::move(bindings))
    {
    }

    void InputAction::beginFrame() noexcept
    {
        runtime.wasTriggered = false;
        for (auto& binding : bindings)
        {
            binding.beginFrame();
        }
    }

    void InputAction::endFrame() noexcept
    {
        glm::vec2 sum{ 0.f };
        bool fired = false;
        bool pressed = false;

        for (const auto& binding : bindings)
        {
            sum += binding.getContribution();
            fired |= binding.runtime.firedThisFrame;
            pressed |= binding.trigger == InputTrigger::Down && binding.runtime.wasPressedThisFrame;
        }

        const bool wasActive = runtime.isActive;
        runtime.value = sum;
        runtime.isActive = sum.x != 0.f || sum.y != 0.f;
        runtime.wasTriggered = fired || pressed || (runtime.isActive && !wasActive);
    }

    void InputAction::resetRuntime() noexcept
    {
        runtime = {};
        for (auto& binding : bindings)
        {
            binding.runtime = {};
        }
    }

    void to_json(nlohmann::json& j, const InputBinding& v)
    {
        j = R<InputBinding>::Serialize(v).getData();
    }

    void from_json(const nlohmann::json& j, InputBinding& v)
    {
        const RResourceStream<RJsonResourceStream> data(j);
        R<InputBinding>::Deserialize(data, v);
    }

    void to_json(nlohmann::json& j, const InputAction& v)
    {
        j = R<InputAction>::Serialize(v).getData();
    }

    void from_json(const nlohmann::json& j, InputAction& v)
    {
        const RResourceStream<RJsonResourceStream> data(j);
        R<InputAction>::Deserialize(data, v);
    }
} // namespace NX
