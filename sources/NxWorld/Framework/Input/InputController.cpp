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
        GetInputSystem().registerController(this);
    }

    void InputController::onDeinitialize()
    {
        GetInputSystem().unregisterController(this);
        for (auto& action : _actions)
        {
            action.resetRuntime();
        }
        BaseComponent::onDeinitialize();
    }

    bool InputController::addAction(InputAction action)
    {
        if (action.name.isEmpty() || findAction(action.name))
        {
            return false;
        }
        // The action may be a copy of another controller's live action.
        action.resetRuntime();
        _actions.emplace_back(std::move(action));
        return true;
    }

    bool InputController::addAction(const Core::StringAtom& name,
                                    std::vector<InputBinding> bindings)
    {
        return addAction(InputAction{ name, std::move(bindings) });
    }

    bool InputController::addAction(const Core::StringAtom& name,
                                    std::vector<InputBinding> bindings, ActionCallback onTriggered)
    {
        if (!addAction(name, std::move(bindings)))
        {
            return false;
        }
        this->onTriggered(name, std::move(onTriggered));
        return true;
    }

    void InputController::onTriggered(const Core::StringAtom& action, ActionCallback callback)
    {
        if (callback)
        {
            _triggeredCallbacks[action].push_back(std::move(callback));
        }
    }

    void InputController::clearCallbacks(const Core::StringAtom& action)
    {
        _triggeredCallbacks.erase(action);
    }

    bool InputController::removeAction(const Core::StringAtom& name)
    {
        const auto it = std::ranges::find(_actions, name, &InputAction::name);
        if (it == _actions.end())
        {
            return false;
        }
        _actions.erase(it);
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
        const auto* found = findActionOrWarn(action);
        return found && isEnabled() && found->runtime.isActive;
    }

    bool InputController::wasTriggered(const Core::StringAtom& action) const
    {
        const auto* found = findActionOrWarn(action);
        return found && isEnabled() && found->runtime.wasTriggered;
    }

    glm::vec2 InputController::getVectorValue(const Core::StringAtom& action) const
    {
        const auto* found = findActionOrWarn(action);
        return found && isEnabled() ? found->runtime.value : glm::vec2{ 0.f };
    }

    const InputAction* InputController::findActionOrWarn(const Core::StringAtom& name) const
    {
        const auto* action = findAction(name);
        if (!action && _reportedMissingActions.insert(name).second)
        {
            warnLog("Unknown input action '{}'"_f << name);
        }
        return action;
    }

    void InputController::beginInputFrame()
    {
        for (auto& action : _actions)
        {
            action.beginFrame();
        }
    }

    bool InputController::onButton(InputSource source, std::uint16_t code, InputState state,
                                   InputModifier mods)
    {
        bool accepted = false;
        for (auto& action : _actions)
        {
            for (auto& binding : action.bindings)
            {
                if (binding.matches(source, code))
                {
                    binding.handleButton(state, mods);
                    accepted |= state == InputState::Pressed && binding.acceptsModifiers(mods);
                }
            }
        }
        return accepted;
    }

    bool InputController::onVector(InputSource source, glm::vec2 value)
    {
        bool accepted = false;
        for (auto& action : _actions)
        {
            for (auto& binding : action.bindings)
            {
                if (binding.matches(source, 0))
                {
                    binding.handleVector(value);
                    accepted = true;
                }
            }
        }
        return accepted;
    }

    bool InputController::onAxis(std::uint16_t axis, float value)
    {
        const float normalized = NormalizeAxis(axis, value);
        bool accepted = false;
        for (auto& action : _actions)
        {
            for (auto& binding : action.bindings)
            {
                if (binding.matches(InputSource::GamepadAxis, axis))
                {
                    binding.handleAxis(normalized);
                    accepted = true;
                }
            }
        }
        return accepted;
    }

    void InputController::releaseHeldInput()
    {
        for (auto& action : _actions)
        {
            action.releaseHeld();
        }
    }

    void InputController::endInputFrame()
    {
        for (auto& action : _actions)
        {
            action.endFrame();
        }
    }

    void InputController::dispatchTriggeredCallbacks()
    {
        if (!isEnabled() || _triggeredCallbacks.empty())
        {
            return;
        }

        std::vector<ActionCallback> callbacks;
        for (const auto& action : _actions)
        {
            if (!action.runtime.wasTriggered)
            {
                continue;
            }

            const auto it = _triggeredCallbacks.find(action.name);
            if (it != _triggeredCallbacks.end())
            {
                callbacks.insert(callbacks.end(), it->second.begin(), it->second.end());
            }
        }

        // A callback may remove actions or destroy this controller, so the callbacks are
        // copied first and members are not touched after the first call.
        for (const auto& callback : callbacks)
        {
            callback();
        }
    }
} // namespace NX
