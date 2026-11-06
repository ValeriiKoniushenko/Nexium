// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "NxWorld/Framework/Input/InputController.h"

#include "NxWorld/Framework/InputSystem.h"
#include "NxWorld/PrivateModuleInfo.h"
#include "Platform/Input/InputDevices/Joystick.h"

#include <algorithm>

namespace
{
    constexpr auto MatchedModifiers = NX::InputModifier::Shift | NX::InputModifier::Control
                                      | NX::InputModifier::Alt | NX::InputModifier::Super;

    bool IsButtonSource(NX::InputSource source)
    {
        return source == NX::InputSource::Key || source == NX::InputSource::MouseButton
               || source == NX::InputSource::GamepadButton;
    }

    bool IsPerFrameSource(NX::InputSource source)
    {
        return source == NX::InputSource::MouseDelta || source == NX::InputSource::MouseScroll;
    }

    float NormalizeAxis(std::uint16_t axis, float value)
    {
        const float rest = Platform::Joystick::getAxisRestValue(axis);
        return rest == 0.f ? value : (value - rest) / (1.f - rest);
    }
} // namespace

namespace NX
{
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

    ECS_IMPL(NX::InputController);

    InputController::~InputController()
    {
        GetInputSystem().unregisterController(this);
    }

    spdlog::logger* InputController::getLogger() const
    {
        return NxWorld::getLogger();
    }

    void InputController::onInitialize()
    {
        BaseComponent::onInitialize();
        ensureStatesMatchActions();
        GetInputSystem().registerController(this);
    }

    void InputController::onDeinitialize()
    {
        GetInputSystem().unregisterController(this);
        BaseComponent::onDeinitialize();
    }

    bool InputController::addAction(InputAction action)
    {
        if (action.name.isEmpty() || findAction(action.name))
        {
            return false;
        }
        _actions.emplace_back(std::move(action));
        ensureStatesMatchActions();
        return true;
    }

    bool InputController::removeAction(const Core::StringAtom& name)
    {
        const auto it = std::ranges::find(_actions, name, &InputAction::name);
        if (it == _actions.end())
        {
            return false;
        }
        _actions.erase(it);
        ensureStatesMatchActions();
        return true;
    }

    const InputAction* InputController::findAction(const Core::StringAtom& name) const
    {
        const auto it = std::ranges::find(_actions, name, &InputAction::name);
        return it != _actions.end() ? &*it : nullptr;
    }

    InputAction* InputController::findAction(const Core::StringAtom& name)
    {
        const auto it = std::ranges::find(_actions, name, &InputAction::name);
        return it != _actions.end() ? &*it : nullptr;
    }

    bool InputController::isActive(const Core::StringAtom& action) const
    {
        const auto* state = findState(action);
        return state && isEnabled() && state->isActive;
    }

    bool InputController::wasTriggered(const Core::StringAtom& action) const
    {
        const auto* state = findState(action);
        return state && isEnabled() && state->wasTriggered;
    }

    glm::vec2 InputController::getVectorValue(const Core::StringAtom& action) const
    {
        const auto* state = findState(action);
        return state && isEnabled() ? state->value : glm::vec2{ 0.f };
    }

    const InputController::ActionState* InputController::findState(
        const Core::StringAtom& action) const
    {
        const auto it = std::ranges::find(_actions, action, &InputAction::name);
        if (it == _actions.end())
        {
            if (_reportedMissingActions.insert(action).second)
            {
                warnLog("Unknown input action '{}'"_f << action);
            }
            return nullptr;
        }

        const auto index = static_cast<std::size_t>(std::distance(_actions.begin(), it));
        return index < _actionStates.size() ? &_actionStates[index] : nullptr;
    }

    bool InputController::statesMatchActions() const
    {
        if (_actionStates.size() != _actions.size())
        {
            return false;
        }

        for (std::size_t i = 0; i < _actions.size(); ++i)
        {
            if (_actionStates[i].bindings.size() != _actions[i].bindings.size())
            {
                return false;
            }
        }
        return true;
    }

    void InputController::ensureStatesMatchActions()
    {
        if (statesMatchActions())
        {
            return;
        }

        _actionStates.assign(_actions.size(), ActionState{});
        for (std::size_t i = 0; i < _actions.size(); ++i)
        {
            _actionStates[i].bindings.resize(_actions[i].bindings.size());
        }
    }

    template<typename Function>
    void InputController::forEachBinding(const Function& function)
    {
        ensureStatesMatchActions();
        for (std::size_t i = 0; i < _actions.size(); ++i)
        {
            for (std::size_t j = 0; j < _actions[i].bindings.size(); ++j)
            {
                function(_actions[i].bindings[j], _actionStates[i].bindings[j]);
            }
        }
    }

    void InputController::beginInputFrame()
    {
        forEachBinding(
            [](const InputBinding& binding, BindingState& state)
            {
                state.wasPressedThisFrame = false;
                state.firedThisFrame = false;
                if (IsPerFrameSource(binding.source))
                {
                    state.value = glm::vec2{ 0.f };
                }
            });

        for (auto& action : _actionStates)
        {
            action.wasTriggered = false;
        }
    }

    void InputController::onButton(InputSource source, std::uint16_t code, InputState state,
                                   InputModifier mods)
    {
        forEachBinding(
            [&](const InputBinding& binding, BindingState& bindingState)
            {
                if (binding.source != source || binding.code != code)
                {
                    return;
                }

                const bool modifiersMatch = (mods & MatchedModifiers) == binding.getModifiers();
                if (state == InputState::Pressed && modifiersMatch)
                {
                    bindingState.isHeld = true;
                    bindingState.wasPressedThisFrame = true;
                    bindingState.firedThisFrame |= binding.trigger == InputTrigger::Pressed;
                }
                else if (state == InputState::Released && bindingState.isHeld)
                {
                    bindingState.isHeld = false;
                    bindingState.firedThisFrame |= binding.trigger == InputTrigger::Released;
                }
            });
    }

    void InputController::onVector(InputSource source, glm::vec2 value)
    {
        forEachBinding(
            [&](const InputBinding& binding, BindingState& state)
            {
                if (binding.source == source)
                {
                    state.value += value;
                }
            });
    }

    void InputController::onAxis(std::uint16_t axis, float value)
    {
        const float normalized = NormalizeAxis(axis, value);
        forEachBinding(
            [&](const InputBinding& binding, BindingState& state)
            {
                if (binding.source == InputSource::GamepadAxis && binding.code == axis)
                {
                    state.value = glm::vec2{ normalized, 0.f };
                }
            });
    }

    void InputController::endInputFrame()
    {
        ensureStatesMatchActions();
        for (std::size_t i = 0; i < _actions.size(); ++i)
        {
            const auto& action = _actions[i];
            auto& actionState = _actionStates[i];

            glm::vec2 sum{ 0.f };
            bool firedThisFrame = false;
            bool pressedThisFrame = false;

            for (std::size_t j = 0; j < action.bindings.size(); ++j)
            {
                const auto& binding = action.bindings[j];
                const auto& state = actionState.bindings[j];

                glm::vec2 contribution = state.value;
                if (IsButtonSource(binding.source))
                {
                    const bool isDownTrigger = binding.trigger == InputTrigger::Down;
                    const bool isOn = isDownTrigger ? state.isHeld : state.firedThisFrame;
                    contribution = glm::vec2{ isOn ? 1.f : 0.f, 0.f };
                    firedThisFrame |= state.firedThisFrame;
                    pressedThisFrame |= isDownTrigger && state.wasPressedThisFrame;
                }

                if (binding.negate)
                {
                    contribution = -contribution;
                }
                if (binding.swizzleYX)
                {
                    contribution = glm::vec2{ contribution.y, contribution.x };
                }
                sum += contribution;
            }

            const bool wasActive = actionState.isActive;
            actionState.value = sum;
            actionState.isActive = sum.x != 0.f || sum.y != 0.f;
            actionState.wasTriggered
                = firedThisFrame || ((actionState.isActive || pressedThisFrame) && !wasActive);
        }
    }
} // namespace NX
