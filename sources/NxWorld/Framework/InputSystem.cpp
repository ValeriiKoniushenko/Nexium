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
#include "Platform/Window.h"

#include <ranges>

namespace
{
    NX::InputModifier ConvertModifiers(int mods)
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
        return result;
    }

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
    void InputSystem::initialize(Platform::Window& window)
    {
        if (_initialized)
        {
            return;
        }

        _subscriptions << window.onKeyPressed->subscribeAndGetID(
            [this](Platform::Keyboard::Key key, int scancode, Platform::Keyboard::KeyState state,
                   int mods) { pushKeyEvent(key, scancode, state, mods); });
        _subscriptions << window.onMouseKeyPressed->subscribeAndGetID(
            [this](Platform::Mouse::Key button, Platform::Mouse::State state,
                   Platform::Mouse::Mod mods) { pushMouseEvent(button, state, mods); });
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
    void InputSystem::pushKeyEvent(Platform::Keyboard::Key key, int scancode,
                                   Platform::Keyboard::KeyState state, int mods)
    {
        _events.push_back({ .key = key,
                            .state = state,
                            .modifiers = ConvertModifiers(mods),
                            .scancode = scancode,
                            .pressedKeys = {} });
    }
    void InputSystem::pushMouseEvent(Platform::Mouse::Key button, Platform::Mouse::State state,
                                     Platform::Mouse::Mod mods)
    {
        if (button == Platform::Mouse::Key::None
            || (state != Platform::Mouse::State::Press && state != Platform::Mouse::State::Release))
        {
            return;
        }

        _events.push_back({ .key = button,
                            .state = state == Platform::Mouse::State::Press
                                         ? Platform::Keyboard::KeyState::Pressed
                                         : Platform::Keyboard::KeyState::Released,
                            .modifiers = ConvertModifiers(static_cast<int>(mods)) });
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

        if (InputCapture::isActive())
        {
            releaseControllerActions();
        }

        while (!_events.empty())
        {
            const auto event = _events.front();
            _events.pop_front();
            dispatch(event);
        }
    }

    void InputSystem::dispatch(const KeyInputEvent& event)
    {
        const bool captured = InputCapture::isActive();
        InputCapture::updateButton(event.key, event.state);
        auto routedEvent = event;
        if (!routedEvent.key.isMouse())
        {
            routedEvent.key = NormalizeModifier(routedEvent.key.getKeyboardKey());
        }
        if (event.state == Platform::Keyboard::KeyState::Pressed
            && std::ranges::find(_pressedKeys, routedEvent.key) == _pressedKeys.end())
        {
            _pressedKeys.push_back(routedEvent.key);
        }
        routedEvent.pressedKeys = _pressedKeys;

        if (event.state == Platform::Keyboard::KeyState::Released)
        {
            std::erase(_pressedKeys, routedEvent.key);
        }

        if (captured || InputCapture::isActive())
        {
            releaseControllerActions();
            return;
        }

        const auto controllers = _routedControllers;
        for (auto* controller : controllers)
        {
            if (InputCapture::isActive())
            {
                releaseControllerActions();
                return;
            }
            if (std::ranges::find(_routedControllers, controller) != _routedControllers.end()
                && controller->isEnabled())
            {
                InputController::Ptr keepAlive
                    = controller->getHardRefCount() ? controller : nullptr;
                controller->handleRoutedEvent(routedEvent);
            }
        }
    }
    std::vector<InputController*>& InputSystem::controllersFor(InputContext context)
    {
        return context == InputContext::Editor ? _editorControllers : _gameplayControllers;
    }
    const std::vector<InputController*>& InputSystem::controllersFor(InputContext context) const
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
        InputCapture::resetButtons();
        releaseControllerActions();
    }

    void InputSystem::releaseControllerActions()
    {
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
    std::vector<InputController*> InputSystem::selectControllers() const
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
