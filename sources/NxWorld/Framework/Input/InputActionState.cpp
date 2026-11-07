// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "NxWorld/Framework/Input/InputActionState.h"

namespace NX
{
    void BindingState::beginFrame(const InputBinding& binding)
    {
        wasPressedThisFrame = false;
        firedThisFrame = false;
        if (binding.isPerFrame())
        {
            value = glm::vec2{ 0.f };
        }
    }

    void BindingState::onButton(const InputBinding& binding, InputState state, InputModifier mods)
    {
        if (state == InputState::Pressed && binding.acceptsModifiers(mods))
        {
            isHeld = true;
            wasPressedThisFrame = true;
            firedThisFrame |= binding.trigger == InputTrigger::Pressed;
        }
        else if (state == InputState::Released && isHeld)
        {
            isHeld = false;
            firedThisFrame |= binding.trigger == InputTrigger::Released;
        }
    }

    void BindingState::addVector(glm::vec2 delta)
    {
        value += delta;
    }

    void BindingState::setAxis(float normalizedValue)
    {
        value = glm::vec2{ normalizedValue, 0.f };
    }

    glm::vec2 BindingState::contribution(const InputBinding& binding) const
    {
        if (!binding.isButton())
        {
            return binding.applyValueModifiers(value);
        }

        const bool isOn = binding.trigger == InputTrigger::Down ? isHeld : firedThisFrame;
        return binding.applyValueModifiers(glm::vec2{ isOn ? 1.f : 0.f, 0.f });
    }

    ActionState::ActionState(const InputAction& action)
        : bindings(action.bindings.size())
    {
    }

    bool ActionState::fits(const InputAction& action) const noexcept
    {
        return bindings.size() == action.bindings.size();
    }

    void ActionState::beginFrame(const InputAction& action)
    {
        wasTriggered = false;
        for (std::size_t i = 0; i < bindings.size(); ++i)
        {
            bindings[i].beginFrame(action.bindings[i]);
        }
    }

    void ActionState::endFrame(const InputAction& action)
    {
        glm::vec2 sum{ 0.f };
        bool fired = false;
        bool pressed = false;

        for (std::size_t i = 0; i < bindings.size(); ++i)
        {
            const auto& binding = action.bindings[i];
            const auto& state = bindings[i];

            sum += state.contribution(binding);
            fired |= state.firedThisFrame;
            pressed |= binding.trigger == InputTrigger::Down && state.wasPressedThisFrame;
        }

        const bool wasActive = isActive;
        value = sum;
        isActive = sum.x != 0.f || sum.y != 0.f;
        wasTriggered = fired || pressed || (isActive && !wasActive);
    }
} // namespace NX
