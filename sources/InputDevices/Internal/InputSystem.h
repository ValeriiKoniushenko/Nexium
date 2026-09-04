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
#pragma once

#include "Core/Delegate.h"
#include "GameplaySystem/Framework/InputController.h"
#include "InputDevices/InputTypes.h"

#include <deque>
#include <vector>

namespace Core
{
    class Window;

    class InputSystem final : public Singleton<InputSystem>
    {
    public:
        /**
         * @brief InputSystem::initialize - Subscribes the input system to window keyboard events.
         *
         * Registers a keyboard event listener and keeps its subscription alive for the lifetime of
         * the input system. Repeated initialization calls are ignored.
         *
         * @param window The window that produces keyboard events.
         */
        void initialize(Window& window);

        /**
         * @brief InputSystem::processEvents - Processes one frame of queued keyboard input.
         *
         * Resets transient actions on routed controllers and dispatches all queued events.
         Controller

         * * routing is updated by context and registration events rather than recalculated here.
         */
        void processEvents();

        /**
         * @brief InputSystem::setActiveContext - Selects the context that receives keyboard input.
         *
         * Rebuilds the routed-controller snapshot immediately when the context changes and releases
         * actions owned by controllers that no longer receive input.
         *
         * @param context The input context that should become active.
         */
        void setActiveContext(InputContext context);

        /**
         * @brief InputSystem::registerController - Registers a controller for its fixed input
         * context.
         *
         * Adds the controller to its context without replacing other controllers already registered
         * there. Expired controller references are removed during registration.
         *
         * @param controller The controller to register. A null pointer is ignored.
         */
        void registerController(InputController* controller);
        void unregisterController(InputController* controller);

    private:
        /**
         * @brief InputSystem::pushKeyEvent - Converts and queues a raw keyboard event.
         *
         * Normalizes left and right modifier keys, converts GLFW modifier flags, and stores the
         * event for processing during the next input frame.
         *
         * @param key The keyboard key associated with the event.
         * @param scancode The platform-specific keyboard scancode.
         * @param state The current state of the key.
         * @param mods The GLFW modifier flags active when the event was produced.
         */
        void pushKeyEvent(Keyboard::Key key, int scancode, Keyboard::KeyState state, int mods);

        /**
         * @brief InputSystem::dispatch - Updates keyboard state and routes an event to the
         * controller.
         *
         * Maintains the normalized set of currently pressed keys, attaches that snapshot to the
         * routed event, and forwards it to every enabled controller selected for this frame.
         *
         * @param event The queued keyboard event to dispatch.
         */
        void dispatch(const KeyInputEvent& event);

        /**
         * @brief InputSystem::selectControllers - Selects enabled controllers for active routing.
         *
         * Resolves every live controller assigned to the current input context and excludes
         * disabled controllers.
         *
         * @return The controllers selected for routing. The collection is empty when none are
         * active.
         */
        [[nodiscard]] std::vector<InputController*> selectControllers() const;

        /**
         * @brief InputSystem::controllersFor - Returns the controller list for an input context.
         *
         * Provides read-only access to the editor or gameplay controller list.
         *
         * @param context The context whose controllers should be returned.
         * @return A constant reference to the matching non-owning controller list.
         */
        [[nodiscard]] std::vector<InputController*>& controllersFor(InputContext context);

        [[nodiscard]] const std::vector<InputController*>& controllersFor(
            InputContext context) const;

        void refreshRoutedControllers();

        std::deque<KeyInputEvent> _events;

        /// Non-owning pointers. Every controller unregisters itself before destruction.
        /// Controllers registered for shortcuts that are active while editor UI owns input.
        std::vector<InputController*> _editorControllers;
        /// Controllers registered for gameplay actions such as player or spectator movement.
        std::vector<InputController*> _gameplayControllers;
        /// Live snapshot of the active context. Context changes rebuild it immediately; events
        /// are dispatched only to these controllers until the context changes again.
        std::vector<InputController*> _routedControllers;
        InputContext _activeContext = InputContext::Editor;

        std::vector<Keyboard::Key> _pressedKeys;

        DelegateSubscriberPoolGuard _subscriptions;

        bool _initialized = false;
    };

    [[nodiscard]] InputSystem* GetInputSystem();
} // namespace Core
