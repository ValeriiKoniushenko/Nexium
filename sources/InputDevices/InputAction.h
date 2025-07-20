// MIT License
//
// Copyright (c) 2019-2025 Valerii Koniushenko
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include "Core/StringHelper.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "glm/glm.hpp"

#include <Core/Delegate.h>
#include <boost/intrusive_ptr.hpp>
#include <boost/smart_ptr/intrusive_ref_counter.hpp>
#include <chrono>

namespace SW
{
    /**
     * @brief Base input action class for handling generic key inputs.
     *
     * @tparam _KeyT Type representing a key (e.g., int, enum, etc.).
     */
    template<class _KeyT>
    class InputAction : public boost::intrusive_ref_counter<InputAction<_KeyT>>
    {
    private:
        enum class State
        {
            None,
            Pressed
        };

    public:
        using Self = InputAction<_KeyT>;
        using Ptr = boost::intrusive_ptr<Self>;
        using CPtr = boost::intrusive_ptr<const Self>;
        template<bool isConst>
        using AdaptivePtr = std::conditional_t<isConst, CPtr, Ptr>;
        using TimeT = std::chrono::milliseconds;
        using KeyT = _KeyT;

        struct SpecKeysState
        {
            Keyboard::KeyState leftShift = Keyboard::KeyState::None;
            Keyboard::KeyState leftAlt = Keyboard::KeyState::None;
            Keyboard::KeyState leftCtrl = Keyboard::KeyState::None;

            static SpecKeysState fillAndGet()
            {
                return { .leftShift = Keyboard::getKeyState(GLFW_KEY_LEFT_SHIFT),
                         .leftAlt = Keyboard::getKeyState(GLFW_KEY_LEFT_ALT),
                         .leftCtrl = Keyboard::getKeyState(GLFW_KEY_LEFT_CONTROL) };
            }
        };

    public:
        InputAction() = default;

        explicit InputAction(const Core::StringAtom& name)
            : _name(name)
        {
        }

        InputAction(const Core::StringAtom& name, KeyT key)
            : _name(name),
              _key(key)
        {
        }
        virtual ~InputAction() = default;

        [[nodiscard]] const Core::StringAtom& getName() const { return _name; }

        void setName(const Core::StringAtom& newName) { _name = newName; }

        void setFrequency(TimeT value) { _frequency = value; }

        [[nodiscard]] TimeT getFrequency() const { return _frequency; }

        virtual void update()
        {
            if (_key)
            {
                if (isKeyPressed())
                {
                    if (_lastState != State::Pressed || _isRepeatable)
                    {
                        if (std::chrono::duration_cast<TimeT>(std::chrono::system_clock::now()
                                                              - _lastUpdate)
                            >= _frequency)
                        {
                            SpecKeysState specs = SpecKeysState::fillAndGet();

                            onPress.trigger(specs);
                            _onActionPrivate.trigger(specs);

                            _lastUpdate = std::chrono::system_clock::now();
                            _lastState = State::Pressed;
                        }
                    }
                }
                else
                {
                    _lastState = State::None;
                }
            }
        }

        void setKey(KeyT key) { _key = key; }

        [[nodiscard]] std::optional<KeyT> getKey() const { return _key; }

        [[nodiscard]] bool getIsRepeatable() const { return _isRepeatable; }

        void setIsRepeatable(bool isRepeatable) { _isRepeatable = isRepeatable; }

        /**
         * @brief will be called while pressing on the needed button.
         * @param SpecKeysState states of special keys
         */
        Core::Delegate<void(SpecKeysState)> onPress;

    protected:
        [[nodiscard]] virtual bool isKeyPressed() const = 0;

        /**
         * @brief will be called while pressing on the needed button.
         * @param SpecKeysState states of special keys
         */
        Core::Delegate<void(SpecKeysState)> _onActionPrivate;
        std::optional<typename decltype(_onActionPrivate)::ID> _idActionPrivate;

    protected:
        Core::StringAtom _name;
        std::optional<KeyT> _key{};
        TimeT _frequency = TimeT(0);
        std::chrono::system_clock::time_point _lastUpdate{};
        State _lastState = State::None;
        bool _isRepeatable = true;
    };

    /**
     * @brief Handles input actions specifically from the keyboard.
     * Also, can be called as KeyboardIA.
     * Better to create it using SW::KeyboardInputManger. I.e.:
     */
    class KeyboardInputAction : public InputAction<int>
    {
    public:
        using Parent = InputAction;
        using Self = KeyboardInputAction;
        using Ptr = boost::intrusive_ptr<Self>;
        using CPtr = boost::intrusive_ptr<const Self>;
        using KeyT = Parent::KeyT;

        static Ptr Create() { return { new Self }; }

        KeyboardInputAction() = default;
        KeyboardInputAction(const Core::StringAtom& name, int key);

    protected:
        [[nodiscard]] bool isKeyPressed() const override;
    };

    /**
     * @brief Handles input actions specifically from the mouse.
     * Also, can be called as MousedIA
     * Better to create it using SW::MouseInputManger. I.e.:
     */
    class MouseInputAction : public InputAction<int>
    {
    public:
        using Parent = InputAction;
        using Self = MouseInputAction;
        using Ptr = boost::intrusive_ptr<Self>;
        using CPtr = boost::intrusive_ptr<const Self>;
        using KeyT = Parent::KeyT;

        static Ptr Create() { return { new Self }; }

        MouseInputAction();
        MouseInputAction(const Core::StringAtom& name, int key);
        explicit MouseInputAction(const Core::StringAtom& name);

        Core::Delegate<void(glm::vec2, SpecKeysState)> onMove;
        Core::Delegate<void(glm::vec2, SpecKeysState)> onMouseClick;

        void update() override;

    protected:
        [[nodiscard]] bool isKeyPressed() const override;

    private:
        void init();

        std::optional<glm::vec2> _lastMousePosition = {};
    };

    using KeyboardIA = KeyboardInputAction;
    using MouseIA = MouseInputAction;

    template<class T>
    concept IsInputAction = requires(T) {
        typename T::KeyT;
        typename T::Ptr;
        typename T::CPtr;
        typename T::Parent;
        std::derived_from<T, SW::InputAction<typename T::KeyT>>;
    };
} // namespace SW