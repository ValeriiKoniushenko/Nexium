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

#include "InputAction.h"

namespace SW
{
    KeyboardInputAction::KeyboardInputAction(const Core::StringAtom& name, int key)
        : InputAction(name, key)
    {
    }

    bool KeyboardInputAction::isKeyPressed() const
    {
        if (_key)
        {
            return Keyboard::isKeyPressed(_key.value());
        }
        return false;
    }

    MouseInputAction::MouseInputAction()
    {
        init();
    }

    MouseInputAction::MouseInputAction(const Core::StringAtom& name, int key)
        : InputAction(name, key)
    {
        init();
    }

    MouseInputAction::MouseInputAction(const Core::StringAtom& name)
        : InputAction(name)
    {
        init();
    }

    bool MouseInputAction::isKeyPressed() const
    {
        if (_key)
        {
            return Mouse::isKeyPressed(_key.value());
        }
        return false;
    }

    void MouseInputAction::update()
    {
        InputAction::update();

        if (Mouse::getPosition() != _lastMousePosition)
        {
            onMove.trigger(Mouse::getPosition() - _lastMousePosition);
            _lastMousePosition = Mouse::getPosition();
        }
    }

    void MouseInputAction::init()
    {
        _idActionPrivate = _onActionPrivate.subscribeAndGetID(
            [this]()
            {
                onMouseClick.trigger(Mouse::getPosition());
            });
    }

} // namespace SW