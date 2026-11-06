// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "Keyboard.h"

#include "Platform/Input/Window.h"

namespace Platform
{
    Keyboard::KeyState Keyboard::getKeyState(Key key)
    {
        switch (glfwGetKey(GetWindow().getRawWindow(), static_cast<int>(key)))
        {
            case GLFW_PRESS:
                return KeyState::Pressed;
            case GLFW_RELEASE:
                return KeyState::Released;
            case GLFW_REPEAT:
                return KeyState::Repeated;
            default:
                return KeyState::None;
        }
    }

    bool Keyboard::IsKeyPressed(Key key)
    {
        return glfwGetKey(GetWindow().getRawWindow(), static_cast<int>(key)) == GLFW_PRESS;
    }

    bool Keyboard::isKeyRepeated(Key key)
    {
        return glfwGetKey(GetWindow().getRawWindow(), static_cast<int>(key)) == GLFW_REPEAT;
    }

    bool Keyboard::IsKeyReleased(Key key)
    {
        return glfwGetKey(GetWindow().getRawWindow(), static_cast<int>(key)) == GLFW_RELEASE;
    }
} // namespace Platform