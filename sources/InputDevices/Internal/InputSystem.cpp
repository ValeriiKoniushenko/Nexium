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
     * Rebuilds the routed-controller snapshot immediately when the context changes and releases

     * * actions owned by controllers that no longer receive input.
     *
     * @param context The input context that should become active.
     */
    void InputSystem::setActiveContext(InputContext context)
    {
        if (_activeContext == context)
        {
            return;
        }

        _activeContext = context;
        refreshRoutedControllers();
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

    /**
     * @brief InputSystem::processEvents - Processes one frame of queued keyboard input.
     *
     * Resets transient actions on routed controllers and dispatches all queued events. Controller

     * * routing is updated by context and registration events rather than recalculated here.
 */
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

        for (auto* controller : _routedControllers)
        {
            if (controller->isEnabled())
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
     * @return A mutable reference to the matching non-owning controller list.
     */
    std::vector<InputController*>& InputSystem::controllersFor(InputContext context)
    {
        return context == InputContext::Editor ? _editorControllers : _gameplayControllers;
    }

    /**
     * @brief InputSystem::controllersFor - Returns the controller list for an input context.
     *
     * Provides read-only access to the editor or gameplay controller list.
     *
     * @param context The context whose controllers should be returned.
     * @return A constant reference to the matching non-owning controller list.
     */
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

    /**
     * @brief InputSystem::selectControllers - Selects enabled controllers for active routing.
     *
     * Resolves every live controller assigned to the current input context and excludes disabled
     * controllers.
     *
     * @return The controllers selected for routing. The collection is empty when none are active.
     */
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
