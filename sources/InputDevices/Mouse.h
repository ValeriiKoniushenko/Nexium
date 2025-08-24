// MIT License
//
// Copyright (c) 2023 Valerii Koniushenko
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

#include "Graphics/OpenGL.h"
#include "glm/glm.hpp"

#include <Core/String.h>

namespace Core
{

    class Mouse
    {
    public:
        enum class Mod
        {
            None,
            Shift = GLFW_MOD_SHIFT,
            Control = GLFW_MOD_CONTROL,
            Alt = GLFW_MOD_ALT,
            Super = GLFW_MOD_SUPER,
            CapsLock = GLFW_MOD_CAPS_LOCK,
            NumLock = GLFW_MOD_NUM_LOCK
        };

        enum Key
        {
            Key_None,
            Key_1 = GLFW_MOUSE_BUTTON_1,
            Key_2 = GLFW_MOUSE_BUTTON_2,
            Key_3 = GLFW_MOUSE_BUTTON_3,
            Key_4 = GLFW_MOUSE_BUTTON_4,
            Key_5 = GLFW_MOUSE_BUTTON_5,
            Key_6 = GLFW_MOUSE_BUTTON_6,
            Key_7 = GLFW_MOUSE_BUTTON_7,
            Key_8 = GLFW_MOUSE_BUTTON_8,
            Key_Last = GLFW_MOUSE_BUTTON_LAST,
            Key_Left = GLFW_MOUSE_BUTTON_LEFT,
            Key_Right = GLFW_MOUSE_BUTTON_RIGHT,
            Key_Middle = GLFW_MOUSE_BUTTON_MIDDLE
        };

        enum class State
        {
            None,
            Press = GLFW_PRESS,
            Release = GLFW_RELEASE,
        };

        Mouse() = delete;

        [[nodiscard]] static StringAtom KeyToString(Key key);
        [[nodiscard]] static Key FromStringToKey(const StringAtom& str);

        [[nodiscard]] static glm::vec2 GetPosition();

        [[nodiscard]] static bool IsKeyPressed(Key key);
        [[nodiscard]] static bool IsKeyReleased(Key key);
        [[nodiscard]] static bool isKeyRepeated(Key key);
    };

} // namespace Core
