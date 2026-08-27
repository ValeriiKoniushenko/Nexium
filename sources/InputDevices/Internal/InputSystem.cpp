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

namespace Core::Internal
{
    namespace
    {
        [[nodiscard]] InputModifier ConvertModifiers(int mods)
        {
            auto result = InputModifier::None;
            if (mods & GLFW_MOD_SHIFT)
            {
                result = result | InputModifier::Shift;
            }
            if (mods & GLFW_MOD_CONTROL)
            {
                result = result | InputModifier::Control;
            }
            if (mods & GLFW_MOD_ALT)
            {
                result = result | InputModifier::Alt;
            }
            if (mods & GLFW_MOD_SUPER)
            {
                result = result | InputModifier::Super;
            }
            return result;
        }

        [[nodiscard]] Keyboard::Key NormalizeModifier(Keyboard::Key key)
        {
            using Key = Keyboard::Key;
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
        if (!controller || !controller->isEnabled())
        {
            return;
        }

        auto& contextController = controllerFor(controller->getInputContext());
        if (!contextController)
        {
            activateController(controller);
        }
    }

    void InputSystem::activateController(InputController* controller)
    {
        if (!controller)
        {
            return;
        }

        auto& contextController = controllerFor(controller->getInputContext());
        if (auto active = contextController.tryLoad(); active && active.get() != controller)
        {
            active->releaseAllActions();
        }

        contextController = WeakPtr<InputController>(controller);
    }

    void InputSystem::deactivateController(InputController* controller)
    {
        auto& contextController = controllerFor(controller->getInputContext());
        auto active = contextController.tryLoad();
        if (!active || active.get() != controller)
        {
            return;
        }

        active->releaseAllActions();
        contextController = {};
    }

    void InputSystem::processEvents()
    {
        auto controller = selectController();
        auto previouslyRouted = _routedController.tryLoad();
        if (previouslyRouted && (!controller || previouslyRouted.get() != controller.get()))
        {
            previouslyRouted->releaseAllActions();
        }

        _routedController = controller ? WeakPtr<InputController>(controller) : WeakPtr<InputController>{};
        if (controller)
        {
            controller->beginInputFrame();
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

        if (auto controller = _routedController.tryLoad();
            controller && controller->isEnabled())
        {
            controller->handleRoutedEvent(routedEvent);
        }
    }

    WeakPtr<InputController>& InputSystem::controllerFor(InputContext context)
    {
        return context == InputContext::Editor ? _editorController : _gameplayController;
    }

    const WeakPtr<InputController>& InputSystem::controllerFor(InputContext context) const
    {
        return context == InputContext::Editor ? _editorController : _gameplayController;
    }

    IntrusivePtr<InputController> InputSystem::selectController() const
    {
        if (auto controller = controllerFor(_activeContext).tryLoad();
            controller && controller->isEnabled())
        {
            return IntrusivePtr<InputController>(controller.get());
        }

        return nullptr;
    }
} // namespace Core::Internal
