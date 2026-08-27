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
#include "GameplaySystem/Framework/InputController.h"
#include "InputDevices/InputTypes.h"

#include <deque>
#include <vector>

namespace Core
{
    class Window;

    /// @brief Engine implementation details hidden from gameplay-facing input APIs.
    namespace Internal
    {
        class InputSystem final : public Singleton<InputSystem>
        {
        public:
            /// @brief Subscribes the input system to keyboard events produced by a window.
            void initialize(Window& window);

            /// @brief Starts an input frame, updates controllers and dispatches queued events.
            void processEvents();

            void setActiveContext(InputContext context) noexcept { _activeContext = context; }

            [[nodiscard]] InputContext getActiveContext() const noexcept { return _activeContext; }

            void registerController(InputController* controller);

            void activateController(InputController* controller);

            void deactivateController(InputController* controller);

        private:
            /// @brief Converts and queues a raw window keyboard event.
            void pushKeyEvent(Keyboard::Key key, int scancode, Keyboard::KeyState state, int mods);

            void dispatch(const KeyInputEvent& event);

            [[nodiscard]] IntrusivePtr<InputController> selectController() const;
            [[nodiscard]] WeakPtr<InputController>& controllerFor(InputContext context);

            [[nodiscard]] const WeakPtr<InputController>& controllerFor(InputContext context) const;

            /// @brief Keyboard events waiting to be processed during the next input frame.
            std::deque<KeyInputEvent> _events;

            WeakPtr<InputController> _editorController;
            WeakPtr<InputController> _gameplayController;
            WeakPtr<InputController> _routedController;
            InputContext _activeContext = InputContext::Editor;

            /// @brief Normalized keyboard keys currently held by the user.
            std::vector<Keyboard::Key> _pressedKeys;

            /// @brief Keeps window input subscriptions alive for the lifetime of the system.
            DelegateSubscriberPoolGuard _subscriptions;

            /// @brief Prevents subscribing to the window more than once.
            bool _initialized = false;
        };

    } // namespace Internal
} // namespace Core
