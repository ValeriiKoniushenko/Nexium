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

// clang-format off
#include "glad/glad.h"
#include "GLFW/glfw3.h"
// clang-format on

#include "JustReflectMe/Adapter.h"

namespace Core
{
    class Keyboard final
    {
    public:
        ENUM_CLASS();
        enum class Key : std::uint16_t
        {
            None = 0,
            Space = GLFW_KEY_SPACE,
            Apostrophe = GLFW_KEY_APOSTROPHE,
            Comma = GLFW_KEY_COMMA,
            Minus = GLFW_KEY_MINUS,
            Period = GLFW_KEY_PERIOD,
            Slash = GLFW_KEY_SLASH,
            _0 = GLFW_KEY_0,
            _1 = GLFW_KEY_1,
            _2 = GLFW_KEY_2,
            _3 = GLFW_KEY_3,
            _4 = GLFW_KEY_4,
            _5 = GLFW_KEY_5,
            _6 = GLFW_KEY_6,
            _7 = GLFW_KEY_7,
            _8 = GLFW_KEY_8,
            _9 = GLFW_KEY_9,
            Semicolon = GLFW_KEY_SEMICOLON,
            Equal = GLFW_KEY_EQUAL,
            A = GLFW_KEY_A,
            B = GLFW_KEY_B,
            C = GLFW_KEY_C,
            D = GLFW_KEY_D,
            E = GLFW_KEY_E,
            F = GLFW_KEY_F,
            G = GLFW_KEY_G,
            H = GLFW_KEY_H,
            I = GLFW_KEY_I,
            J = GLFW_KEY_J,
            K = GLFW_KEY_K,
            L = GLFW_KEY_L,
            M = GLFW_KEY_M,
            N = GLFW_KEY_N,
            O = GLFW_KEY_O,
            P = GLFW_KEY_P,
            Q = GLFW_KEY_Q,
            R = GLFW_KEY_R,
            S = GLFW_KEY_S,
            T = GLFW_KEY_T,
            U = GLFW_KEY_U,
            V = GLFW_KEY_V,
            W = GLFW_KEY_W,
            X = GLFW_KEY_X,
            Y = GLFW_KEY_Y,
            Z = GLFW_KEY_Z,
            Left_Bracket = GLFW_KEY_LEFT_BRACKET,
            Backslash = GLFW_KEY_BACKSLASH,
            Right_Bracket = GLFW_KEY_RIGHT_BRACKET,
            Grave_Accent = GLFW_KEY_GRAVE_ACCENT,
            World_1 = GLFW_KEY_WORLD_1,
            World_2 = GLFW_KEY_WORLD_2,
            Escape = GLFW_KEY_ESCAPE,
            Enter = GLFW_KEY_ENTER,
            Tab = GLFW_KEY_TAB,
            Backspace = GLFW_KEY_BACKSPACE,
            Insert = GLFW_KEY_INSERT,
            Delete = GLFW_KEY_DELETE,
            Right = GLFW_KEY_RIGHT,
            Left = GLFW_KEY_LEFT,
            Down = GLFW_KEY_DOWN,
            Up = GLFW_KEY_UP,
            Page_Up = GLFW_KEY_PAGE_UP,
            Page_Down = GLFW_KEY_PAGE_DOWN,
            Home = GLFW_KEY_HOME,
            End = GLFW_KEY_END,
            Caps_Lock = GLFW_KEY_CAPS_LOCK,
            Scroll_Lock = GLFW_KEY_SCROLL_LOCK,
            Num_Lock = GLFW_KEY_NUM_LOCK,
            Print_Screen = GLFW_KEY_PRINT_SCREEN,
            Pause = GLFW_KEY_PAUSE,
            F1 = GLFW_KEY_F1,
            F2 = GLFW_KEY_F2,
            F3 = GLFW_KEY_F3,
            F4 = GLFW_KEY_F4,
            F5 = GLFW_KEY_F5,
            F6 = GLFW_KEY_F6,
            F7 = GLFW_KEY_F7,
            F8 = GLFW_KEY_F8,
            F9 = GLFW_KEY_F9,
            F10 = GLFW_KEY_F10,
            F11 = GLFW_KEY_F11,
            F12 = GLFW_KEY_F12,
            F13 = GLFW_KEY_F13,
            F14 = GLFW_KEY_F14,
            F15 = GLFW_KEY_F15,
            F16 = GLFW_KEY_F16,
            F17 = GLFW_KEY_F17,
            F18 = GLFW_KEY_F18,
            F19 = GLFW_KEY_F19,
            F20 = GLFW_KEY_F20,
            F21 = GLFW_KEY_F21,
            F22 = GLFW_KEY_F22,
            F23 = GLFW_KEY_F23,
            F24 = GLFW_KEY_F24,
            F25 = GLFW_KEY_F25,
            Kp_0 = GLFW_KEY_KP_0,
            Kp_1 = GLFW_KEY_KP_1,
            Kp_2 = GLFW_KEY_KP_2,
            Kp_3 = GLFW_KEY_KP_3,
            Kp_4 = GLFW_KEY_KP_4,
            Kp_5 = GLFW_KEY_KP_5,
            Kp_6 = GLFW_KEY_KP_6,
            Kp_7 = GLFW_KEY_KP_7,
            Kp_8 = GLFW_KEY_KP_8,
            Kp_9 = GLFW_KEY_KP_9,
            Kp_Decimal = GLFW_KEY_KP_DECIMAL,
            Kp_Divide = GLFW_KEY_KP_DIVIDE,
            Kp_Multiply = GLFW_KEY_KP_MULTIPLY,
            Kp_Subtract = GLFW_KEY_KP_SUBTRACT,
            Kp_Add = GLFW_KEY_KP_ADD,
            Kp_Enter = GLFW_KEY_KP_ENTER,
            Kp_Equal = GLFW_KEY_KP_EQUAL,
            Left_Shift = GLFW_KEY_LEFT_SHIFT,
            Left_Control = GLFW_KEY_LEFT_CONTROL,
            Left_Alt = GLFW_KEY_LEFT_ALT,
            Left_Super = GLFW_KEY_LEFT_SUPER,
            Right_Shift = GLFW_KEY_RIGHT_SHIFT,
            Right_Control = GLFW_KEY_RIGHT_CONTROL,
            Right_Alt = GLFW_KEY_RIGHT_ALT,
            Right_Super = GLFW_KEY_RIGHT_SUPER,
            Menu = GLFW_KEY_MENU,
            Last = GLFW_KEY_LAST,
        };

        ENUM_CLASS();
        enum class KeyState : std::uint8_t
        {
            None = 0,
            Pressed = GLFW_PRESS,
            Released = GLFW_RELEASE,
            Repeated = GLFW_REPEAT
        };

        Keyboard() = delete;

        [[nodiscard]] static KeyState getKeyState(Key key);

        [[nodiscard]] static bool IsKeyPressed(Key key);

        [[nodiscard]] static bool isKeyRepeated(Key key);

        [[nodiscard]] static bool IsKeyReleased(Key key);
    };
} // namespace Core

#include "Keyboard.generated.h" // added by the code generator. Better don't move it.
