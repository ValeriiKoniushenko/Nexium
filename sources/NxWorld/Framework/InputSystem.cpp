// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
#include "InputSystem.h"

#include "NxWorld/Framework/Input/InputController.h"
#include "Platform/Input/Events/InputEvent.h"
#include "Platform/Input/Window.h"

#include <ranges>

namespace NX
{

    void InputSystem::initialize(Platform::Window& window)
    {
        if (_initialized)
        {
            return;
        }

        auto registerEvent
            = [this](auto&& event) { queueEvent(std::forward<decltype(event)>(event)); };

        _subscriptions << window.onMouseMove->subscribeAndGetID(registerEvent);
        _subscriptions << window.onKeyPressed->subscribeAndGetID(registerEvent);
        _subscriptions << window.onMouseWheel->subscribeAndGetID(registerEvent);
        _subscriptions << window.onFocusChanged->subscribeAndGetID(registerEvent);
        _subscriptions << window.onMouseKeyPressed->subscribeAndGetID(registerEvent);
        _subscriptions << window.onJoystickConnected->subscribeAndGetID(registerEvent);
        _subscriptions << window.onJoystickAxisChanged->subscribeAndGetID(registerEvent);
        _subscriptions << window.onJoystickButtonPressed->subscribeAndGetID(registerEvent);

        _initialized = true;
    }

    void InputSystem::resetInput()
    {
        _events.clear();
        _pressedKeys.clear();
    }

    void InputSystem::registerController(InputController* controller)
    {
        if (controller && !isRegistered(controller))
        {
            _controllers.push_back(controller);
        }
    }

    void InputSystem::unregisterController(InputController* controller)
    {
        std::erase(_controllers, controller);
    }

    bool InputSystem::isRegistered(const InputController* controller) const
    {
        return std::ranges::find(_controllers, controller) != _controllers.end();
    }
    // tag::input_process_events[]
    void InputSystem::processEvents()
    {
        // Handlers may queue new events; they go to _events and are processed next frame.
        _processing.clear();
        std::swap(_events, _processing);

        for (auto* controller : _controllers)
        {
            controller->beginInputFrame();
        }

        for (const auto& event : _processing)
        {
            std::visit([this](const auto& concreteEvent) { handle(concreteEvent); }, event);
        }

        for (auto* controller : _controllers)
        {
            controller->endInputFrame();
        }

        _processing.clear();
    }
    // end::input_process_events[]

    void InputSystem::handle(const KeyboardEvent& event)
    {
        for (auto* controller : _controllers)
        {
            controller->onButton(InputSource::Key, event.getKey().getIndex(), event.getState(),
                                 event.getMods());
        }
    }

    void InputSystem::handle(const TextInputEvent&)
    {
    }

    void InputSystem::handle(const MouseMoveEvent& event)
    {
        for (auto* controller : _controllers)
        {
            controller->onVector(InputSource::MouseDelta, event.getDelta());
        }
    }

    void InputSystem::handle(const MouseButtonEvent& event)
    {
        for (auto* controller : _controllers)
        {
            controller->onButton(InputSource::MouseButton, event.getButton().getIndex(),
                                 event.getState(), event.getMods());
        }
    }

    void InputSystem::handle(const MouseScrollEvent& event)
    {
        for (auto* controller : _controllers)
        {
            controller->onVector(InputSource::MouseScroll, event.getOffset());
        }
    }

    void InputSystem::handle(const MouseEnterEvent&)
    {
    }

    void InputSystem::handle(const GamepadButtonEvent& event)
    {
        for (auto* controller : _controllers)
        {
            controller->onButton(InputSource::GamepadButton, event.getButton().getIndex(),
                                 event.getState(), InputModifier::None);
        }
    }

    void InputSystem::handle(const GamepadAxisEvent& event)
    {
        for (auto* controller : _controllers)
        {
            controller->onAxis(event.getAxis(), event.getValue());
        }
    }

    void InputSystem::handle(const GamepadConnectionEvent&)
    {
    }

    void InputSystem::handle(const WindowResizeEvent&)
    {
    }

    void InputSystem::handle(const WindowFocusEvent&)
    {
    }

    void InputSystem::handle(const WindowIconifyEvent&)
    {
    }

    void InputSystem::handle(const WindowCloseEvent&)
    {
    }

    InputSystem& GetInputSystem()
    {
        return InputSystem::Instance();
    }
} // namespace NX
