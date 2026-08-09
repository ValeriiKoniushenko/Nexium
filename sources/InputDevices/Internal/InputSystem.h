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
#include "Core/IntrusivePtr.h"
#include "InputDevices/InputTypes.h"

#include <cstdint>
#include <deque>
#include <unordered_map>
#include <vector>

namespace Core
{
    class InputController;
    class Window;

    /// @brief Engine implementation details hidden from gameplay-facing input APIs.
    namespace Internal
    {
        /// @brief Collects raw keyboard events and routes them through active InputControllers.
        class InputSystem final
        {
        public:
            /// @brief Subscribes the input system to keyboard events produced by a window.
            void initialize(Window& window);

            /// @brief Starts an input frame, updates controllers and dispatches queued events.
            void processEvents();

            /// @brief Adds an initialized InputController to the routing registry.
            void registerController(InputController* controller);

            /// @brief Moves a controller to the newest position in automatic routing order.
            void activateController(InputController* controller);

        private:
            /// @brief Converts and queues a raw window keyboard event.
            void pushKeyEvent(Keyboard::Key key, int scancode, Keyboard::KeyState state, int mods);

            /// @brief Routes one normalized event according to blocking, specificity and recency.
            void dispatch(const KeyInputEvent& event);

            /// @brief Removes destroyed controllers and invalid event owners.
            void removeExpiredControllers();

            /// @brief Keyboard events waiting to be processed during the next input frame.
            std::deque<KeyInputEvent> _events;

            /// @brief Non-owning registry of all initialized InputController components.
            std::vector<WeakPtr<InputController>> _controllers;

            /// @brief Controllers that own pressed trigger keys and receive their repeat events.
            std::unordered_map<Keyboard::Key, WeakPtr<InputController>> _keyOwners;

            /// @brief Normalized keyboard keys currently held by the user.
            std::vector<Keyboard::Key> _pressedKeys;

            /// @brief Monotonic counter used to assign automatic controller activation order.
            std::uint64_t _nextActivationOrder = 0;

            /// @brief Keeps window input subscriptions alive for the lifetime of the system.
            DelegateSubscriberPoolGuard _subscriptions;

            /// @brief Prevents subscribing to the window more than once.
            bool _initialized = false;
        };

        /// @brief Returns the engine-owned singleton input router.
        [[nodiscard]] InputSystem& GetInputSystem();
    } // namespace Internal
} // namespace Core
