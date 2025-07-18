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

#include "Keyboard.h"
#include "Mouse.h"
#include "glm/glm.hpp"

#include <Core/Delegate.h>
#include <chrono>

namespace SW
{

    template<class KeyT>
    class InputAction
    {
    private:
        enum class State
        {
            None,
            Pressed
        };

    public:
        using TimeT = std::chrono::milliseconds;

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
                            onPress.trigger();
                            _onActionPrivate.trigger();
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

        [[nodiscard]] KeyT getKey() const { return _key; }

        [[nodiscard]] bool getIsRepeatable() const { return _isRepeatable; }

        void setIsRepeatable(bool isRepeatable) { _isRepeatable = isRepeatable; }

        Core::Delegate<void()> onPress;

    protected:
        [[nodiscard]] virtual bool isKeyPressed() const = 0;
        Core::Delegate<void()> _onActionPrivate;
        std::optional<Core::Delegate<void()>::ID> _idActionPrivate;

    protected:
        Core::StringAtom _name;
        std::optional<KeyT> _key{};
        TimeT _frequency = TimeT(1);
        std::chrono::system_clock::time_point _lastUpdate{};
        State _lastState = State::None;
        bool _isRepeatable = true;
    };

    class KeyboardInputAction : public InputAction<int>
    {
    public:
        KeyboardInputAction() = default;
        KeyboardInputAction(const Core::StringAtom& name, int key);

    protected:
        [[nodiscard]] bool isKeyPressed() const override;
    };

    class MouseInputAction : public InputAction<int>
    {
    public:
        MouseInputAction();
        MouseInputAction(const Core::StringAtom& name, int key);
        explicit MouseInputAction(const Core::StringAtom& name);

        Core::Delegate<void(glm::ivec2)> onMove;
        Core::Delegate<void(glm::ivec2)> onMouseClick;

        void update() override;

    protected:
        [[nodiscard]] bool isKeyPressed() const override;

    private:
        void init();

        glm::vec2 _lastMousePosition = Mouse::getPosition();
    };

} // namespace SW