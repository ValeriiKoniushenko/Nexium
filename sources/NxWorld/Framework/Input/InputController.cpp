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
    float NormalizeAxis(std::uint16_t axis, float value)
    {
        const float rest = Platform::Joystick::getAxisRestValue(axis);
        return rest == 0.f ? value : (value - rest) / (1.f - rest);
    }
} // namespace

namespace NX
{
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
        _actionStates.clear();
        ensureStatesMatchActions();
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

    const ActionState* InputController::findState(const Core::StringAtom& action) const
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
            if (!_actionStates[i].fits(_actions[i]))
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

        _actionStates.clear();
        for (const auto& action : _actions)
        {
            _actionStates.emplace_back(action);
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
        ensureStatesMatchActions();
        for (std::size_t i = 0; i < _actions.size(); ++i)
        {
            _actionStates[i].beginFrame(_actions[i]);
        }
    }

    void InputController::onButton(InputSource source, std::uint16_t code, InputState state,
                                   InputModifier mods)
    {
        forEachBinding(
            [&](const InputBinding& binding, BindingState& bindingState)
            {
                if (binding.matches(source, code))
                {
                    bindingState.onButton(binding, state, mods);
                }
            });
    }

    void InputController::onVector(InputSource source, glm::vec2 value)
    {
        forEachBinding(
            [&](const InputBinding& binding, BindingState& bindingState)
            {
                if (binding.matches(source, 0))
                {
                    bindingState.addVector(value);
                }
            });
    }

    void InputController::onAxis(std::uint16_t axis, float value)
    {
        const float normalized = NormalizeAxis(axis, value);
        forEachBinding(
            [&](const InputBinding& binding, BindingState& bindingState)
            {
                if (binding.matches(InputSource::GamepadAxis, axis))
                {
                    bindingState.setAxis(normalized);
                }
            });
    }

    void InputController::endInputFrame()
    {
        ensureStatesMatchActions();
        for (std::size_t i = 0; i < _actions.size(); ++i)
        {
            _actionStates[i].endFrame(_actions[i]);
        }
    }
} // namespace NX
