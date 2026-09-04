/*
 * MIT License
 *
 * Copyright (c) 2018-2027 Valerii Koniushenko
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include "InputSystem.h"

#include "GLFW/glfw3.h"
#include "GameplaySystem/Framework/InputController.h"
#include "Graphics/Window.h"

#include <ranges>

namespace
{
    Core::InputModifier ConvertModifiers(int mods)
    {
        auto result = Core::InputModifier::None;
        if (mods & GLFW_MOD_SHIFT)
        {
            result = result | Core::InputModifier::Shift;
        }
        if (mods & GLFW_MOD_CONTROL)
        {
            result = result | Core::InputModifier::Control;
        }
        if (mods & GLFW_MOD_ALT)
        {
            result = result | Core::InputModifier::Alt;
        }
        if (mods & GLFW_MOD_SUPER)
        {
            result = result | Core::InputModifier::Super;
        }
        return result;
    }

    Core::Keyboard::Key NormalizeModifier(Core::Keyboard::Key key)
    {
        using Key = Core::Keyboard::Key;
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

namespace Core
{
    void InputSystem::initialize(Window& window)
    {
        if (_initialized)
        {
            return;
        }

        _subscriptions << window.onKeyPressed->subscribeAndGetID(
            [this](Keyboard::Key key, int scancode, Keyboard::KeyState state, int mods)
            { pushKeyEvent(key, scancode, state, mods); });
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
    void InputSystem::pushKeyEvent(Keyboard::Key key, int scancode, Keyboard::KeyState state,
                                   int mods)
    {
        _events.push_back({ .key = NormalizeModifier(key),
                            .state = state,
                            .modifiers = ConvertModifiers(mods),
                            .scancode = scancode });
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
        for (auto* controller : _routedControllers)
        {
            if (controller->isEnabled())
            {
                controller->beginInputFrame();
            }
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
        auto routedEvent = event;
        if (event.state == Keyboard::KeyState::Pressed
            && std::ranges::find(_pressedKeys, event.key) == _pressedKeys.end())
        {
            _pressedKeys.push_back(event.key);
        }
        routedEvent.pressedKeys = _pressedKeys;

        if (event.state == Keyboard::KeyState::Released)
        {
            std::erase(_pressedKeys, event.key);
        }

        for (auto* controller : _routedControllers)
        {
            if (controller->isEnabled())
            {
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

    InputSystem* GetInputSystem()
    {
        return &InputSystem::Instance();
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
} // namespace Core
