// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
#include "InputSystem.h"

#include "GLFW/glfw3.h"
#include "NxWorld/Framework/InputController.h"
#include "Platform/Input/Window.h"

#include <ranges>

namespace
{

    Platform::Keyboard::Key NormalizeModifier(Platform::Keyboard::Key key)
    {
        using Key = Platform::Keyboard::Key;
        if (key == Key::Right_Shift)
        {
            return Key::Left_Shift;
        }
        if (key == Key::Right_Control)
        {
            return Key::Left_Control;
        }
        if (key == Key::Right_Alt)
        {
            return Key::Left_Alt;
        }
        if (key == Key::Right_Super)
        {
            return Key::Left_Super;
        }
        return key;
    }
} // namespace

namespace NX
{

    NX::InputModifier InputSystem::ConvertModifiers(int mods)
    {
        auto result = NX::InputModifier::None;

        if (mods & GLFW_MOD_SHIFT)
        {
            result = result | NX::InputModifier::Shift;
        }

        if (mods & GLFW_MOD_CONTROL)
        {
            result = result | NX::InputModifier::Control;
        }

        if (mods & GLFW_MOD_ALT)
        {
            result = result | NX::InputModifier::Alt;
        }

        if (mods & GLFW_MOD_SUPER)
        {
            result = result | NX::InputModifier::Super;
        }

        if (mods & GLFW_MOD_CAPS_LOCK)
        {
            result = result | NX::InputModifier::CapsLock;
        }

        if (mods & GLFW_MOD_NUM_LOCK)
        {
            result = result | NX::InputModifier::NumLock;
        }

        return result;
    }

    void InputSystem::initialize(Platform::Window& window)
    {
        if (_initialized)
        {
            return;
        }

        _subscriptions << window.onKeyPressed->subscribeAndGetID(
            [this](NX::InputButton key, int scancode, NX::InputState state, NX::InputModifier mods)
            { pushKeyEvent(key, state, mods, scancode); });
        _subscriptions << window.onMouseKeyPressed->subscribeAndGetID(
            [this](NX::InputButton key, NX::InputState state, NX::InputModifier mods)
            { pushKeyEvent(key, state, mods); });

        _subscriptions << window.onFocusChanged->subscribeAndGetID(
            [this](bool focused)
            {
                if (!focused)
                {
                    resetInput();
                }
            });
        _initialized = true;
    }

    void InputSystem::setActiveContext(InputContext context)
    {
        if (_activeContext == context)
        {
            return;
        }

        _activeContext = context;
        refreshRoutedControllers();
    }

    void InputSystem::pushKeyEvent(NX::InputButton key, NX::InputState state,
                                   NX::InputModifier mods, int scancode)
    {
        _events.emplace_back(
            InputEvent{ .button = key, .state = state, .mods = mods, .scancode = scancode });
    }

    void InputSystem::registerController(InputController* controller)
    {
        if (!controller)
        {
            return;
        }

        auto& controllers = controllersFor(controller->getInputContext());
        if (std::ranges::find(controllers, controller) != controllers.end())
        {
            return;
        }
        controllers.emplace_back(controller);
        if (controller->getInputContext() == _activeContext)
        {
            refreshRoutedControllers();
        }
    }

    void InputSystem::unregisterController(InputController* controller)
    {
        if (!controller)
        {
            return;
        }

        std::erase(_editorControllers, controller);
        std::erase(_gameplayControllers, controller);
        std::erase(_routedControllers, controller);
    }
    void InputSystem::processEvents()
    {
        refreshRoutedControllers();
        for (auto* controller : _routedControllers)
        {
            if (controller->isEnabled())
            {
                controller->beginInputFrame();
            }
        }

        while (!_events.empty())
        {
            const InputEvent event = _events.front();
            _events.pop_front();
            dispatch(event);
        }
    }
    void InputSystem::dispatch(const InputEvent& event)
    {
        auto routedEvent = event;
        if (event.state == InputState::Pressed
            && std::ranges::find(_pressedKeys, event.button) == _pressedKeys.end())
        {
            _pressedKeys.push_back(event.button);
        }
        routedEvent.allPressedButtons = _pressedKeys;

        if (event.state == InputState::Released)
        {
            std::erase(_pressedKeys, event.button);
        }

        const auto controllers = _routedControllers;
        for (auto* controller : controllers)
        {
            if (std::ranges::find(_routedControllers, controller) != _routedControllers.end()
                && controller->isEnabled())
            {
                InputController::Ptr keepAlive
                    = controller->getHardRefCount() ? controller : nullptr;

                // NEW IMPL
                // controller->handleRoutedEvent(routedEvent);
            }
        }
    }
    std::vector<InputController*>& InputSystem::controllersFor(InputContext context)
    {
        return context == InputContext::Editor ? _editorControllers : _gameplayControllers;
    }

    InputSystem& GetInputSystem()
    {
        return InputSystem::Instance();
    }

    void InputSystem::resetInput()
    {
        _events.clear();
        _pressedKeys.clear();
        for (auto* controller : _editorControllers)
        {
            controller->releaseAllActions();
        }
        for (auto* controller : _gameplayControllers)
        {
            controller->releaseAllActions();
        }
    }

    void InputSystem::refreshRoutedControllers()
    {
        auto controllers = selectControllers();
        for (auto* previous : _routedControllers)
        {
            if (std::ranges::find(controllers, previous) == controllers.end())
            {
                previous->releaseAllActions();
            }
        }

        _routedControllers = std::move(controllers);
    }

    std::vector<InputController*> InputSystem::selectControllers()
    {
        std::vector<InputController*> selected;
        for (auto* controller : controllersFor(_activeContext))
        {
            if (controller->isEnabled())
            {
                selected.emplace_back(controller);
            }
        }
        return selected;
    }
} // namespace NX
