// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "InputController.h"

#include "InputSystem.h"

namespace NX
{
    ECS_IMPL(InputController);

    InputController::InputController(const InputController& other)
        : BaseComponent(other),
          speed(other.speed),
          mouseSensitivity(other.mouseSensitivity),
          _inputContext(other._inputContext),
          _bindings(other._bindings),
          _inputEnabled(other._inputEnabled)
    {
        invalidate();
    }

    InputController::~InputController()
    {
        GetInputSystem().unregisterController(this);
    }

    InputController::Ptr InputController::Create(const Core::StringAtom& name, InputContext context)
    {
        Ptr controller = new InputController(name, context);
        controller->initialize();
        return controller;
    }

    bool InputController::bind(const Core::StringAtom& action, const KeyChord& chord,
                               InputActionTrigger trigger)
    {
        return bind({ .action = action, .chord = chord, .trigger = trigger });
    }

    bool InputController::bind(const Core::StringAtom& action, KeyChord chord,
                               ActionCallback callback, InputActionTrigger trigger)
    {
        const Binding binding{ .action = action, .chord = std::move(chord), .trigger = trigger };
        if (!InputMap::IsValid(binding))
        {
            return false;
        }
        const auto inserted = bind(binding);
        if (callback)
        {
            setActionCallback(action, std::move(callback));
        }
        return inserted;
    }

    bool InputController::bind(Binding binding)
    {
        if (!InputMap::IsValid(binding))
        {
            return false;
        }
        cancelInput();
        const auto inserted = InputMap::Bind(_bindings, std::move(binding));
        GetInputSystem().refreshController(this);
        return inserted;
    }

    void InputController::setActionCallback(const Core::StringAtom& action, ActionCallback callback)
    {
        if (action.isEmpty())
        {
            return;
        }
        if (callback)
        {
            _actionCallbacks.insert_or_assign(action, std::move(callback));
        }
        else
        {
            _actionCallbacks.erase(action);
        }
    }

    bool InputController::unbind(const Core::StringAtom& action)
    {
        const auto removed = InputMap::Unbind(_bindings, action);
        if (removed)
        {
            cancelInput();
            GetInputSystem().refreshController(this);
        }
        return removed;
    }

    void InputController::clearBindings()
    {
        cancelInput();
        _bindings.clear();
        GetInputSystem().refreshController(this);
    }

    void InputController::setBindings(const std::vector<Binding>& bindings)
    {
        const auto updated = bindings;
        cancelInput();
        _bindings = updated;
        GetInputSystem().refreshController(this);
    }

    void InputController::setInputContext(InputContext context)
    {
        if (_inputContext != context)
        {
            cancelInput();
            _inputContext = context;
            GetInputSystem().refreshController(this);
        }
    }

    void InputController::setInputEnabled(bool enabled)
    {
        if (_inputEnabled == enabled)
        {
            return;
        }
        _inputEnabled = enabled;
        cancelInput();
        GetInputSystem().refreshController(this);
    }

    bool InputController::isInputEnabled() const noexcept
    {
        if (!_inputEnabled)
        {
            return false;
        }
        for (const auto* component = static_cast<const BaseComponent*>(this); component;
             component = component->getParent())
        {
            if (!component->isEnabled())
            {
                return false;
            }
        }
        return true;
    }

    bool InputController::isActive(const Core::StringAtom& action) const
    {
        return isInputEnabled() && _runtime.isActive(action);
    }

    std::uint8_t InputController::getActionModifiers(const Core::StringAtom& action) const
    {
        return isInputEnabled() ? _runtime.getModifiers(action)
                                : std::uint8_t{ InputModifier_None };
    }

    glm::vec2 InputController::getActionValue(const Core::StringAtom& action) const
    {
        return isInputEnabled() ? _runtime.getValue(action) : glm::vec2{};
    }

    Tag InputController::getTags() const
    {
        return BaseComponent::getTags() | Tag_InputController;
    }

    void InputController::onInitialize()
    {
        BaseComponent::onInitialize();
        cancelInput();
        GetInputSystem().registerController(this);
    }

    void InputController::onPostDeserialize(AbstractComponent* obj, const RLogsCollector& logs)
    {
        BaseComponent::onPostDeserialize(obj, logs);
        cancelInput();
        GetInputSystem().refreshController(this);
    }

    void InputController::onDeinitialize()
    {
        cancelInput();
        GetInputSystem().unregisterController(this);
        BaseComponent::onDeinitialize();
    }

    std::optional<InputController::Binding> InputController::resolveBinding(
        const KeyInputEvent& event) const
    {
        return InputMap::ResolveBinding(_bindings, event);
    }

    void InputController::handleRoutedEvent(const KeyInputEvent& event)
    {
        if (event.type == InputEventType::Button
            && event.state == Platform::Keyboard::KeyState::Released)
        {
            handleReleasedEvent(event);
            return;
        }
        if (const auto binding = resolveBinding(event))
        {
            handleRoutedEvent(event, *binding);
        }
    }

    void InputController::handleRoutedEvent(const KeyInputEvent& event, const Binding& binding)
    {
        if (!isInputEnabled()
            || (event.type == InputEventType::Button
                && event.state != Platform::Keyboard::KeyState::Pressed))
        {
            return;
        }
        if (const auto action = _runtime.activate(binding, event))
        {
            emitAction(*action);
        }
    }

    void InputController::handleReleasedEvent(const KeyInputEvent& event)
    {
        const Ptr keepAlive = getHardRefCount() ? this : nullptr;
        if (!isInputEnabled())
        {
            cancelInput();
            return;
        }
        const auto actions = _runtime.release(event);
        const auto revision = _runtimeRevision;
        for (const auto& action : actions)
        {
            if (!isInputEnabled() || InputCapture::isActive() || revision != _runtimeRevision)
            {
                break;
            }
            emitAction(action);
        }
    }

    void InputController::beginInputFrame()
    {
        if (isInputEnabled())
        {
            _runtime.beginFrame();
        }
        else
        {
            cancelInput();
        }
    }

    void InputController::cancelInput(std::uint8_t channels)
    {
        ++_runtimeRevision;
        _runtime.cancel(channels);
    }

    void InputController::cancelMatchingBindings(
        const std::function<bool(const Binding&)>& predicate)
    {
        ++_runtimeRevision;
        _runtime.cancelMatching(predicate);
    }

    void InputController::emitAction(const InputActionEvent& event)
    {
        // Callbacks may edit handlers or release the last external reference.
        const Ptr keepAlive = getHardRefCount() ? this : nullptr;
        auto delegate = onAction;
        if (const auto found = _actionCallbacks.find(event.action); found != _actionCallbacks.end())
        {
            const auto callback = found->second;
            callback(event);
        }
        if (delegate)
        {
            delegate->trigger(event);
        }
    }
} // namespace NX
