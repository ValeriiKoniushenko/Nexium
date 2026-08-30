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

    namespace Internal
    {
        class InputSystem final : public Singleton<InputSystem>
        {
        public:
            void initialize(Window& window);

            void processEvents();

            void setActiveContext(InputContext context) noexcept;

            void registerController(InputController* controller);

        private:
            void pushKeyEvent(Keyboard::Key key, int scancode, Keyboard::KeyState state, int mods);

            void dispatch(const KeyInputEvent& event);

            [[nodiscard]] std::vector<IntrusivePtr<InputController>> selectControllers() const;
            [[nodiscard]] std::vector<WeakPtr<InputController>>& controllersFor(
                InputContext context);

            [[nodiscard]] const std::vector<WeakPtr<InputController>>& controllersFor(
                InputContext context) const;

            std::deque<KeyInputEvent> _events;

            std::vector<WeakPtr<InputController>> _editorControllers;
            std::vector<WeakPtr<InputController>> _gameplayControllers;
            std::vector<WeakPtr<InputController>> _routedControllers;
            InputContext _activeContext = InputContext::Editor;

            std::vector<Keyboard::Key> _pressedKeys;

            DelegateSubscriberPoolGuard _subscriptions;

            bool _initialized = false;
        };

        [[nodiscard]] InputSystem* GetInputSystem();

    } // namespace Internal
} // namespace Core
