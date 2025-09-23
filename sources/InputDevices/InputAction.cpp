/*
 * MIT License
 *
 * Copyright (c) 2018-2025 Valerii Koniushenko
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

#include "InputAction.h"

#include "Graphics/Window.h"
#include "Keyboard.h"

namespace Core
{
    KeyboardInputAction::KeyboardInputAction(const StringAtom& name, KeyT key)
        : InputAction(name, key)
    {
    }

    bool KeyboardInputAction::isKeyPressed() const
    {
        if (_key && _key != Keyboard::Key_None)
        {
            return Keyboard::IsKeyPressed(_key.value());
        }
        return false;
    }

    MouseInputAction::MouseInputAction()
    {
        init();
    }

    MouseInputAction::MouseInputAction(const StringAtom& name, KeyT key)
        : InputAction(name, key)
    {
        init();
    }

    MouseInputAction::MouseInputAction(const StringAtom& name)
        : InputAction(name)
    {
        init();
    }

    bool MouseInputAction::isKeyPressed() const
    {
        if (_key && _key != Mouse::Key_None)
        {
            return Mouse::IsKeyPressed(_key.value());
        }
        return false;
    }

    void MouseInputAction::update()
    {
        InputAction::update();

        const auto pos = Mouse::GetPosition();
        if (!_lastMousePosition)
        {
            _lastMousePosition = pos;
            return;
        }

        if (gDragDrop.getState() == DragAndDrop::State::Dragging)
        {
            auto k = _key.value();
            if (_key == Mouse::Key_None || (_key != Mouse::Key_None && isKeyPressed()))
            {
                onDrag.trigger(pos - *_lastMousePosition, SpecKeysState::fillAndGet());
            }
        }

        if (pos != *_lastMousePosition)
        {
            onMove.trigger(pos - *_lastMousePosition, SpecKeysState::fillAndGet());
            _lastMousePosition = pos;
        }
    }

    void MouseInputAction::init()
    {
        _onActionPrivate.subscribe(
            [this](SpecKeysState states)
            {
                onMouseClick.trigger(Mouse::GetPosition(), states);
            });
    }

} // namespace Core