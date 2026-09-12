// Nexium
// Copyright 2018-2027 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "InputAction.h"

#include "Platform/Keyboard.h"
#include "Platform/Window.h"

using namespace Platform;

namespace NX
{
    KeyboardInputAction::KeyboardInputAction(const Core::StringAtom& name, KeyT key)
        : InputAction(name, key)
    {
    }

    bool KeyboardInputAction::isKeyPressed() const
    {
        if (_key && _key != Keyboard::Key::None)
        {
            return Keyboard::IsKeyPressed(_key.value());
        }
        return false;
    }

    MouseInputAction::MouseInputAction()
    {
        init();
    }

    MouseInputAction::MouseInputAction(const Core::StringAtom& name, KeyT key)
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
        if (_key && _key != Mouse::Key::None)
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
            if (_key == Mouse::Key::None || (_key != Mouse::Key::None && isKeyPressed()))
            {
                onDrag->trigger(pos - *_lastMousePosition, SpecKeysState::fillAndGet());
            }
        }

        if (pos != *_lastMousePosition)
        {
            onMove->trigger(pos - *_lastMousePosition, SpecKeysState::fillAndGet());
            _lastMousePosition = pos;
        }
    }

    void MouseInputAction::init()
    {
        _subscription = _onActionPrivate->subscribeAndGetID(
            [this](SpecKeysState states)
            { onMouseClick->trigger(Platform::Mouse::GetPosition(), states); });
    }
} // namespace NX
