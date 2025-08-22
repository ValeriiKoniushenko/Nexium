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

namespace Core
{
    /**
     * @brief Base input action class for handling generic key inputs.
     *
     * Represents a generic input action bound to a key or button. Handles
     * repeatable presses, timing, and delegates for when the action occurs.
     *
     * @tparam KeyTParam Type representing a key (e.g., int, enum, etc.).
     */
    template<class KeyTParam>
    class InputAction : public boost::intrusive_ref_counter<InputAction<KeyTParam>>
    {
    private:
        enum class State
        {
            None,
            Pressed
        };

    public:
        using Self = InputAction<KeyTParam>;
        using Ptr = boost::intrusive_ptr<Self>;
        using CPtr = boost::intrusive_ptr<const Self>;
        template<bool isConst>
        using AdaptivePtr = std::conditional_t<isConst, CPtr, Ptr>;
        using TimeT = std::chrono::milliseconds;
        using KeyT = KeyTParam;

        /**
         * @brief Represents the state of special modifier keys (Shift, Alt, Ctrl).
         */
        struct SpecKeysState
        {
            Keyboard::KeyState leftShift = Keyboard::KeyState::None;
            Keyboard::KeyState leftAlt = Keyboard::KeyState::None;
            Keyboard::KeyState leftCtrl = Keyboard::KeyState::None;

            /**
             * @brief Reads the current state of modifier keys and returns it.
             * @return SpecKeysState with current states of Shift, Alt, and Ctrl.
             */
            static SpecKeysState fillAndGet()
            {
                return { .leftShift = Keyboard::getKeyState(Keyboard::Key::Key_Left_Shift),
                         .leftAlt = Keyboard::getKeyState(Keyboard::Key::Key_Left_Alt),
                         .leftCtrl = Keyboard::getKeyState(Keyboard::Key::Key_Left_Control) };
            }
        };

    public:
        InputAction() = default;

        explicit InputAction(StringAtom name)
            : _name(std::move(name))
        {
        }

        InputAction(StringAtom name, KeyT key)
            : _name(std::move(name)),
              _key(key)
        {
        }
        virtual ~InputAction() = default;

        [[nodiscard]] const StringAtom& getName() const { return _name; }

        void setName(const StringAtom& newName) { _name = newName; }

        void setFrequency(TimeT value) { _frequency = value; }

        [[nodiscard]] TimeT getFrequency() const { return _frequency; }

        /**
         * @brief Updates the input action state.
         *
         * Checks if the bound key is pressed and triggers delegates if conditions
         * (repeatable, frequency, etc.) are met.
         */
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
         * @brief Delegate triggered while pressing the assigned key.
         * @param SpecKeysState Current states of special modifier keys.
         */
        Delegate<void(SpecKeysState)> onPress;

    protected:
        /**
         * @brief Checks if the bound key is currently pressed.
         * Must be implemented by derived classes.
         */
        [[nodiscard]] virtual bool isKeyPressed() const = 0;

        /**
         * @brief will be called while pressing on the needed button.
         * @param SpecKeysState states of special keys
         */
        Delegate<void(SpecKeysState)> _onActionPrivate;
        std::optional<typename decltype(_onActionPrivate)::IDGuard> _idActionPrivate;

    protected:
        StringAtom _name;
        std::optional<KeyT> _key{};
        TimeT _frequency = TimeT(0);
        std::chrono::system_clock::time_point _lastUpdate{};
        State _lastState = State::None;
        bool _isRepeatable = true;
    };

    /**
     * @brief Handles input actions specifically from the keyboard.
     * Also, can be called as KeyboardIA.
     * Better to create it using KeyboardInputManger. I.e.:
     */
    class KeyboardInputAction : public InputAction<Keyboard::Key>
    {
    public:
        using Parent = InputAction;
        using Self = KeyboardInputAction;
        using Ptr = boost::intrusive_ptr<Self>;
        using CPtr = boost::intrusive_ptr<const Self>;
        using KeyT = Parent::KeyT;

        static Ptr Create() { return { new Self }; }

        KeyboardInputAction() = default;
        KeyboardInputAction(const StringAtom& name, KeyT key);

    protected:
        [[nodiscard]] bool isKeyPressed() const override;
    };

    /**
     * @brief Handles input actions specifically from the mouse.
     * Also, can be called as MousedIA
     * Better to create it using MouseInputManger. I.e.:
     */
    class MouseInputAction : public InputAction<Mouse::Key>
    {
    public:
        using Parent = InputAction;
        using Self = MouseInputAction;
        using Ptr = boost::intrusive_ptr<Self>;
        using CPtr = boost::intrusive_ptr<const Self>;
        using KeyT = Parent::KeyT;

        static Ptr Create() { return { new Self }; }

        MouseInputAction();
        explicit MouseInputAction(const StringAtom& name, KeyT key = Mouse::Key_None);
        explicit MouseInputAction(const StringAtom& name);

        Delegate<void(glm::vec2, SpecKeysState)> onDrag;
        Delegate<void(glm::vec2, SpecKeysState)> onMove;
        Delegate<void(glm::vec2, SpecKeysState)> onMouseClick;

        void update() override;

    protected:
        [[nodiscard]] bool isKeyPressed() const override;

    private:
        void init();

        std::optional<glm::vec2> _lastMousePosition = {};
    };

    /** @brief Alias for KeyboardInputAction. */
    using KeyboardIA = KeyboardInputAction;

    /** @brief Alias for MouseInputAction. */
    using MouseIA = MouseInputAction;

    /**
     * @brief Concept for compile-time checking of InputAction types.
     */
    template<class T>
    concept IsInputAction = requires(T) {
        typename T::KeyT;
        typename T::Ptr;
        typename T::CPtr;
        typename T::Parent;
        requires std::derived_from<T, InputAction<typename T::KeyT>>;
    };
} // namespace Core