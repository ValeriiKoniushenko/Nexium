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

#pragma once

#include "Core/String.h"
#include "JustReflectMe/Adapter.h"
#include "glm/glm.hpp"

// clang-format off
#include "glad/glad.h"
#include "GLFW/glfw3.h"
// clang-format on

namespace Core
{
    class Mouse
    {
    public:
        ENUM_CLASS();
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

        ENUM_CLASS();
        enum class Key
        {
            _1 = GLFW_MOUSE_BUTTON_1,
            _2 = GLFW_MOUSE_BUTTON_2,
            _3 = GLFW_MOUSE_BUTTON_3,
            _4 = GLFW_MOUSE_BUTTON_4,
            _5 = GLFW_MOUSE_BUTTON_5,
            _6 = GLFW_MOUSE_BUTTON_6,
            _7 = GLFW_MOUSE_BUTTON_7,
            _8 = GLFW_MOUSE_BUTTON_8,
            Last = GLFW_MOUSE_BUTTON_LAST,
            Left = GLFW_MOUSE_BUTTON_LEFT,
            Right = GLFW_MOUSE_BUTTON_RIGHT,
            Middle = GLFW_MOUSE_BUTTON_MIDDLE,
            None = ~0
        };

        ENUM_CLASS();
        enum class State : GLenum
        {
            None = 0,
            Press = GLFW_PRESS,
            Release = GLFW_RELEASE,
        };

        Mouse() = delete;

        [[nodiscard]] static glm::vec2 GetPosition();

        [[nodiscard]] static glm::vec2 GetInViewportPosition();

        [[nodiscard]] static bool IsKeyPressed(Key key);

        [[nodiscard]] static bool IsKeyReleased(Key key);

        [[nodiscard]] static bool isKeyRepeated(Key key);
    };
} // namespace Core

#include "Mouse.generated.h" // added by the code generator. Better don't move it.
