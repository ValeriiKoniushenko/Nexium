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

    /**
     * @brief InputSystem::initialize - Subscribes the input system to window keyboard events.
     *
     * Registers a keyboard event listener and keeps its subscription alive for the lifetime of
     * the input system. Repeated initialization calls are ignored.
     *
     * @param window The window that produces keyboard events.
     */
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

    /**
     * @brief InputSystem::setActiveContext - Selects the context that receives keyboard input.
     *
     * Stores the editor or gameplay context used to select a controller during the next input
     * frame. Any previously routed actions are released by processEvents when routing changes.
     *
     * @param context The input context that should become active.
     */
    void InputSystem::setActiveContext(InputContext context) noexcept
    {
        _activeContext = context;
    }

    /**
     * @brief InputSystem::pushKeyEvent - Converts and queues a raw keyboard event.
     *
     * Normalizes left and right modifier keys, converts GLFW modifier flags, and stores the event
     * for processing during the next input frame.
     *
     * @param key The keyboard key associated with the event.
     * @param scancode The platform-specific keyboard scancode.
     * @param state The current state of the key.
     * @param mods The GLFW modifier flags active when the event was produced.
     */
    void InputSystem::pushKeyEvent(Keyboard::Key key, int scancode, Keyboard::KeyState state,
                                   int mods)
    {
        _events.push_back({ .key = NormalizeModifier(key),
                            .state = state,
                            .modifiers = ConvertModifiers(mods),
                            .scancode = scancode });
    }

    /**
     * @brief InputSystem::registerController - Registers a controller for its fixed input context.
     *
     * Adds the controller to its context without replacing other controllers already registered
     * there. Expired controller references are removed during registration.
     *
     * @param controller The controller to register. A null pointer is ignored.
     */
    void InputSystem::registerController(InputController* controller)
    {
        if (!controller)
        {
            return;
        }

        auto& controllers = controllersFor(controller->getInputContext());
        std::erase_if(controllers,
                      [](const WeakPtr<InputController>& value) { return !value.tryLoad(); });
        if (std::ranges::any_of(controllers,
                                [controller](const WeakPtr<InputController>& value)
                                {
                                    auto registered = value.tryLoad();
                                    return registered && registered.get() == controller;
                                }))
        {
            return;
        }
        controllers.emplace_back(controller);
    }

    /**
     * @brief InputSystem::processEvents - Processes one frame of queued keyboard input.
     *
     * Selects all enabled controllers for the active context, releases controllers removed from
     * routing, resets transient actions, and dispatches all queued events.
     */
    void InputSystem::processEvents()
    {
        auto controllers = selectControllers();
        for (const auto& previousWeak : _routedControllers)
        {
            auto previous = previousWeak.tryLoad();
            if (previous
                && std::ranges::none_of(controllers, [&previous](const auto& controller)
                                        { return controller.get() == previous.get(); }))
            {
                previous->releaseAllActions();
            }
        }

        _routedControllers.clear();
        _routedControllers.reserve(controllers.size());
        for (auto& controller : controllers)
        {
            controller->beginInputFrame();
            _routedControllers.emplace_back(controller);
        }

        while (!_events.empty())
        {
            const auto event = _events.front();
            _events.pop_front();
            dispatch(event);
        }
    }

    /**
     * @brief InputSystem::dispatch - Updates keyboard state and routes an event to the controller.
     *
     * Maintains the normalized set of currently pressed keys, attaches that snapshot to the
     * routed event, and forwards it to every enabled controller selected for this frame.
     *
     * @param event The queued keyboard event to dispatch.
     */
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

        for (const auto& controllerWeak : _routedControllers)
        {
            if (auto controller = controllerWeak.tryLoad(); controller && controller->isEnabled())
            {
                controller->handleRoutedEvent(routedEvent);
            }
        }
    }

    /**
     * @brief InputSystem::controllersFor - Returns the controller list for an input context.
     *
     * Provides mutable access to the editor or gameplay controller list.
     *
     * @param context The context whose controllers should be returned.
     * @return A mutable reference to the matching weak controller list.
     */
    std::vector<WeakPtr<InputController>>& InputSystem::controllersFor(InputContext context)
    {
        return context == InputContext::Editor ? _editorControllers : _gameplayControllers;
    }

    /**
     * @brief InputSystem::controllersFor - Returns the controller list for an input context.
     *
     * Provides read-only access to the editor or gameplay controller list.
     *
     * @param context The context whose controllers should be returned.
     * @return A constant reference to the matching weak controller list.
     */
    const std::vector<WeakPtr<InputController>>& InputSystem::controllersFor(
        InputContext context) const
    {
        return context == InputContext::Editor ? _editorControllers : _gameplayControllers;
    }

    /**
     * @brief InputSystem::selectControllers - Selects enabled controllers for active routing.
     *
     * Resolves every live controller assigned to the current input context and excludes disabled
     * controllers.
     *
     * @return The controllers selected for routing. The collection is empty when none are active.
     */
    std::vector<IntrusivePtr<InputController>> InputSystem::selectControllers() const
    {
        std::vector<IntrusivePtr<InputController>> selected;
        for (const auto& controllerWeak : controllersFor(_activeContext))
        {
            if (auto controller = controllerWeak.tryLoad(); controller && controller->isEnabled())
            {
                selected.emplace_back(controller.get());
            }
        }
        return selected;
    }
} // namespace Core::Internal
