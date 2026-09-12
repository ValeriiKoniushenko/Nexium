// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "Core/Delegate.h"
#include "Core/IntrusivePtr.h"
#include "Core/String.h"
#include "Platform/Keyboard.h"
#include "Platform/Mouse.h"
#include "glm/glm.hpp"

#include <chrono>

namespace NX
{
    /// Base input action class for handling generic key inputs.
    ///
    /// Represents a generic input action bound to a key or button. Handles
    /// repeatable presses, timing, and Core::Delegates for when the action occurs.
    ///
    /// @tparam KeyTParam Type representing a key (e.g., int, enum, etc.).
    template<class KeyTParam>
    class InputAction : public Core::IntrusiveRefCounter<InputAction<KeyTParam>>
    {
    private:
        enum class State
        {
            None,
            Pressed
        };

    public:
        using Self = InputAction;
        using Ptr = Core::IntrusivePtr<Self>;
        using CPtr = Core::IntrusivePtr<const Self>;
        template<bool isConst>
        using AdaptivePtr = std::conditional_t<isConst, CPtr, Ptr>;
        using TimeT = std::chrono::milliseconds;
        using KeyT = KeyTParam;

        /// Represents the state of special modifier keys (Shift, Alt, Ctrl).
        struct SpecKeysState
        {
            Platform::Keyboard::KeyState leftShift = Platform::Keyboard::KeyState::None;
            Platform::Keyboard::KeyState leftAlt = Platform::Keyboard::KeyState::None;
            Platform::Keyboard::KeyState leftCtrl = Platform::Keyboard::KeyState::None;

            /// Reads the current state of modifier keys and returns it.
            /// @return SpecKeysState with current states of Shift, Alt, and Ctrl.
            static SpecKeysState fillAndGet()
            {
                return { .leftShift
                         = Platform::Keyboard::getKeyState(Platform::Keyboard::Key::Left_Shift),
                         .leftAlt
                         = Platform::Keyboard::getKeyState(Platform::Keyboard::Key::Left_Alt),
                         .leftCtrl
                         = Platform::Keyboard::getKeyState(Platform::Keyboard::Key::Left_Control) };
            }
        };

    public:
        InputAction() = default;

        explicit InputAction(Core::StringAtom name)
            : _name(std::move(name))
        {
        }

        InputAction(Core::StringAtom name, KeyT key)
            : _name(std::move(name)),
              _key(key)
        {
        }

        InputAction(const InputAction&) = delete;
        InputAction(InputAction&&) noexcept = delete;
        InputAction& operator=(const InputAction&) = delete;
        InputAction& operator=(InputAction&&) noexcept = delete;

        virtual ~InputAction() = default;

        [[nodiscard]] const Core::StringAtom& getName() const { return _name; }

        void setName(const Core::StringAtom& newName) { _name = newName; }

        void setFrequency(TimeT value) { _frequency = value; }

        [[nodiscard]] TimeT getFrequency() const { return _frequency; }

        /// Updates the input action state.
        ///
        /// Checks if the bound key is pressed and triggers Core::Delegates if conditions
        /// (repeatable, frequency, etc.) are met.
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

                            onPress->trigger(specs);
                            _onActionPrivate->trigger(specs);

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

        /// Core::Delegate triggered while pressing the assigned key.
        /// @param SpecKeysState Current states of special modifier keys.
        Core::Delegate<void(SpecKeysState)>::Ptr onPress
            = Core::Delegate<void(SpecKeysState)>::Create();

    protected:
        /// Checks if the bound key is currently pressed.
        /// Must be implemented by derived classes.
        [[nodiscard]] virtual bool isKeyPressed() const = 0;

        /// will be called while pressing on the necessary button.
        /// @param SpecKeysState states of special keys
        Core::Delegate<void(SpecKeysState)>::Ptr _onActionPrivate
            = Core::Delegate<void(SpecKeysState)>::Create();

    protected:
        Core::StringAtom _name;
        std::optional<KeyT> _key{};
        TimeT _frequency = TimeT(0);
        std::chrono::system_clock::time_point _lastUpdate{};
        State _lastState = State::None;
        bool _isRepeatable = true;
    };

    /// Handles input actions specifically from the keyboard.
    /// Also, can be called as KeyboardIA.
    /// Better to create it using KeyboardInputManger. I.e.:
    class KeyboardInputAction : public InputAction<Platform::Keyboard::Key>
    {
        INTRUSIVE_PTR_ADAPTERS(KeyboardInputAction);

    public:
        using Parent = InputAction;
        using Self = KeyboardInputAction;
        using KeyT = KeyT;

        static Ptr Create() { return { new Self }; }

        KeyboardInputAction() = default;

        KeyboardInputAction(const Core::StringAtom& name, KeyT key);

    protected:
        [[nodiscard]] bool isKeyPressed() const override;
    };

    /// Handles input actions specifically from the mouse.
    /// Also, can be called as MousedIA
    /// Better to create it using MouseInputManger. I.e.:
    class MouseInputAction : public InputAction<Platform::Mouse::Key>
    {
    public:
        using Parent = InputAction;
        using Self = MouseInputAction;
        using Ptr = Core::IntrusivePtr<Self>;
        using CPtr = Core::IntrusivePtr<const Self>;
        using KeyT = KeyT;

        static Ptr Create() { return { new Self }; }

        MouseInputAction();

        explicit MouseInputAction(const Core::StringAtom& name,
                                  KeyT key = Platform::Mouse::Key::None);

        explicit MouseInputAction(const Core::StringAtom& name);

        Core::Delegate<void(glm::vec2, SpecKeysState)>::Ptr onDrag
            = Core::Delegate<void(glm::vec2, SpecKeysState)>::Create();
        Core::Delegate<void(glm::vec2, SpecKeysState)>::Ptr onMove
            = Core::Delegate<void(glm::vec2, SpecKeysState)>::Create();
        Core::Delegate<void(glm::vec2, SpecKeysState)>::Ptr onMouseClick
            = Core::Delegate<void(glm::vec2, SpecKeysState)>::Create();

        void update() override;

    protected:
        [[nodiscard]] bool isKeyPressed() const override;

    private:
        void init();

        Core::DelegateSubscriber _subscription;
        std::optional<glm::vec2> _lastMousePosition = std::nullopt;
    };

    /// Alias for KeyboardInputAction.
    using KeyboardIA = KeyboardInputAction;

    /// Alias for MouseInputAction.
    using MouseIA = MouseInputAction;

    /// Concept for compile-time checking of InputAction types.
    template<class T>
    concept IsInputAction = requires(T) {
        typename T::KeyT;
        typename T::Ptr;
        typename T::CPtr;
        typename T::Parent;
        requires std::derived_from<std::remove_reference_t<T>, InputAction<typename T::KeyT>>;
    };
} // namespace NX
