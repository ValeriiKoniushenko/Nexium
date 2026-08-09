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

#include <algorithm>
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

    InputSystem& GetInputSystem()
    {
        static InputSystem instance;
        return instance;
    }

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
        if (!controller)
        {
            return;
        }

        const auto alreadyRegistered
            = std::ranges::any_of(_controllers,
                                  [controller](const auto& weak)
                                  {
                                      const auto loaded = weak.tryLoad();
                                      return loaded && loaded.get() == controller;
                                  });
        if (!alreadyRegistered)
        {
            _controllers.emplace_back(controller);
            if (controller->isInputActive())
                activateController(controller);
        }
    }

    void InputSystem::activateController(InputController* controller)
    {
        if (!controller)
            return;

        controller->_activationOrder = ++_nextActivationOrder;
    }

    void InputSystem::processEvents()
    {
        removeExpiredControllers();

        for (const auto& weak : _controllers)
        {
            if (auto controller = weak.tryLoad())
            {
                controller->beginInputFrame();
                controller->refreshActiveState();
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
            // A chord ends when either its trigger or any required key is released.
            for (const auto& weak : _controllers)
            {
                if (auto controller = weak.tryLoad())
                {
                    (void)controller->handleRoutedEvent(routedEvent);
                }
            }
            std::erase(_pressedKeys, event.key);
            _keyOwners.erase(event.key);
            return;
        }

        if (event.state == Keyboard::KeyState::Repeated)
        {
            if (const auto ownerIt = _keyOwners.find(event.key); ownerIt != _keyOwners.end())
            {
                if (auto owner = ownerIt->second.tryLoad())
                {
                    (void)owner->handleRoutedEvent(routedEvent);
                }
            }
            return;
        }

        std::vector<IntrusivePtr<InputController>> activeControllers;
        activeControllers.reserve(_controllers.size());
        for (const auto& weak : _controllers)
        {
            if (auto controller = weak.tryLoad(); controller && controller->isInputActive())
            {
                activeControllers.emplace_back(controller.get());
            }
        }

        std::ranges::stable_sort(
            activeControllers,
            [&routedEvent](const auto& lhs, const auto& rhs)
            {
                const bool lhsBlocks
                    = lhs->getBlockingPolicy() == InputBlockingPolicy::BlockKeyboard;
                const bool rhsBlocks
                    = rhs->getBlockingPolicy() == InputBlockingPolicy::BlockKeyboard;
                if (lhsBlocks != rhsBlocks)
                    return lhsBlocks;
                if (lhsBlocks)
                    return lhs->_activationOrder > rhs->_activationOrder;

                const auto lhsSpecificity = lhs->getMatchingSpecificity(routedEvent);
                const auto rhsSpecificity = rhs->getMatchingSpecificity(routedEvent);
                if (lhsSpecificity.has_value() != rhsSpecificity.has_value())
                    return lhsSpecificity.has_value();
                if (lhsSpecificity && *lhsSpecificity != *rhsSpecificity)
                    return *lhsSpecificity > *rhsSpecificity;
                return lhs->_activationOrder > rhs->_activationOrder;
            });

        for (auto& controller : activeControllers)
        {
            if (controller->handleRoutedEvent(routedEvent) == InputResult::Consumed)
            {
                _keyOwners.insert_or_assign(event.key, WeakPtr<InputController>(controller));
                return;
            }
        }
    }

    void InputSystem::removeExpiredControllers()
    {
        std::erase_if(_controllers, [](const auto& weak) { return !weak; });
        std::erase_if(_keyOwners, [](const auto& pair) { return !pair.second; });
    }
} // namespace Core::Internal
