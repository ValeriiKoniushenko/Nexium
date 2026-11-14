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

#include <algorithm>
#include <functional>
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
        _subscriptions << window.onIconifyChanged->subscribeAndGetID(registerEvent);
        _subscriptions << window.onMouseKeyPressed->subscribeAndGetID(registerEvent);
        _subscriptions << window.onJoystickConnected->subscribeAndGetID(registerEvent);
        _subscriptions << window.onJoystickAxisChanged->subscribeAndGetID(registerEvent);
        _subscriptions << window.onJoystickButtonPressed->subscribeAndGetID(registerEvent);

        _window = &window;
        syncWindowState();
        _initialized = true;
    }

    void InputSystem::resetInput()
    {
        _events.clear();
        syncWindowState();
    }

    void InputSystem::syncWindowState()
    {
        // A window that was never created has no state, and counts as active.
        if (_window && _window->getRawWindow())
        {
            setWindowState(_window->isFocused(), _window->isIconified());
        }
        else
        {
            setWindowState(true, false);
        }
    }

    void InputSystem::setWindowState(bool focused, bool iconified)
    {
        const bool wasActive = isWindowActive();
        _isWindowFocused = focused;
        _isWindowIconified = iconified;

        if (wasActive && !isWindowActive())
        {
            // Nothing held can be trusted any more: the matching releases may never arrive, and
            // a stick held at that moment is no longer reported.
            for (auto* controller : _controllers)
            {
                controller->releaseHeldInput();
            }
        }
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

        _ordered = _controllers;
        std::ranges::stable_sort(_ordered, std::greater<>{}, &InputController::getPriority);

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

        // A callback may add or destroy controllers, including ones not visited yet. Walk a
        // snapshot so removing an entry cannot shift a neighbour out of the loop (its one-frame
        // triggers would be lost), and skip controllers that were destroyed meanwhile.
        // Controllers added by a callback are not in the snapshot and start with the next frame.
        _dispatching = _controllers;
        for (auto* controller : _dispatching)
        {
            if (isRegistered(controller))
            {
                controller->dispatchTriggeredCallbacks();
            }
        }
        _dispatching.clear();

        _processing.clear();
    }
    // end::input_process_events[]

    template<typename Deliver>
    void InputSystem::route(bool consumable, Deliver&& deliver)
    {
        bool consumed = false;
        int consumedPriority = 0;
        for (auto* controller : _ordered)
        {
            if (consumed && controller->getPriority() < consumedPriority)
            {
                break;
            }

            const bool accepted = deliver(*controller);
            if (consumable && !consumed && accepted && controller->isEnabled()
                && controller->doesConsumeInput())
            {
                consumed = true;
                consumedPriority = controller->getPriority();
            }
        }
    }

    void InputSystem::routeButton(InputSource source, std::uint16_t code, InputState state,
                                  InputModifier mods)
    {
        // Only a press can start something, so only a press is ignored while the window is
        // inactive or consumed by a higher priority. A release always reaches everyone, otherwise
        // a button could stay held in a controller that never saw its release.
        const bool isPress = state == InputState::Pressed;
        if (isPress && !isWindowActive())
        {
            return;
        }

        route(isPress, [&](InputController& controller)
              { return controller.onButton(source, code, state, mods); });
    }

    void InputSystem::handle(const KeyboardEvent& event)
    {
        routeButton(InputSource::Key, event.getKey().getIndex(), event.getState(), event.getMods());
    }

    void InputSystem::handle(const TextInputEvent&)
    {
    }

    void InputSystem::handle(const MouseMoveEvent& event)
    {
        if (!isWindowActive())
        {
            return;
        }

        route(true, [&](InputController& controller)
              { return controller.onVector(InputSource::MouseDelta, event.getDelta()); });
    }

    void InputSystem::handle(const MouseButtonEvent& event)
    {
        routeButton(InputSource::MouseButton, event.getButton().getIndex(), event.getState(),
                    event.getMods());
    }

    void InputSystem::handle(const MouseScrollEvent& event)
    {
        if (!isWindowActive())
        {
            return;
        }

        route(true, [&](InputController& controller)
              { return controller.onVector(InputSource::MouseScroll, event.getOffset()); });
    }

    void InputSystem::handle(const MouseEnterEvent&)
    {
    }

    void InputSystem::handle(const GamepadButtonEvent& event)
    {
        routeButton(InputSource::GamepadButton, event.getButton().getIndex(), event.getState(),
                    InputModifier::None);
    }

    void InputSystem::handle(const GamepadAxisEvent& event)
    {
        if (!isWindowActive())
        {
            return;
        }

        route(true, [&](InputController& controller)
              { return controller.onAxis(event.getAxis(), event.getValue()); });
    }

    void InputSystem::handle(const GamepadConnectionEvent&)
    {
    }

    void InputSystem::handle(const WindowResizeEvent&)
    {
    }

    void InputSystem::handle(const WindowFocusEvent& event)
    {
        setWindowState(event.isFocused(), _isWindowIconified);
    }

    void InputSystem::handle(const WindowIconifyEvent& event)
    {
        setWindowState(_isWindowFocused, event.isIconified());
    }

    void InputSystem::handle(const WindowCloseEvent&)
    {
    }

    InputSystem& GetInputSystem()
    {
        return InputSystem::Instance();
    }
} // namespace NX
