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

#include "Keyboard.h"

#include "Graphics/Window.h"

namespace Core
{

    StringAtom Keyboard::KeyToString(Key key)
    {
        // clang-format off
        if (key == Key_None) return "None"_atom;
        if (key == Key_Space) return "Space"_atom;
        if (key == Key_Apostrophe) return "Apostrophe"_atom;
        if (key == Key_Comma) return "Comma"_atom;
        if (key == Key_Minus) return "Minus"_atom;
        if (key == Key_Period) return "Period"_atom;
        if (key == Key_Slash) return "Slash"_atom;
        if (key == Key_0) return "0"_atom;
        if (key == Key_1) return "1"_atom;
        if (key == Key_2) return "2"_atom;
        if (key == Key_3) return "3"_atom;
        if (key == Key_4) return "4"_atom;
        if (key == Key_5) return "5"_atom;
        if (key == Key_6) return "6"_atom;
        if (key == Key_7) return "7"_atom;
        if (key == Key_8) return "8"_atom;
        if (key == Key_9) return "9"_atom;
        if (key == Key_Semicolon) return "Semicolon"_atom;
        if (key == Key_Equal) return "Equal"_atom;
        if (key == Key_A) return "A"_atom;
        if (key == Key_B) return "B"_atom;
        if (key == Key_C) return "C"_atom;
        if (key == Key_D) return "D"_atom;
        if (key == Key_E) return "E"_atom;
        if (key == Key_F) return "F"_atom;
        if (key == Key_G) return "G"_atom;
        if (key == Key_H) return "H"_atom;
        if (key == Key_I) return "I"_atom;
        if (key == Key_J) return "J"_atom;
        if (key == Key_K) return "K"_atom;
        if (key == Key_L) return "L"_atom;
        if (key == Key_M) return "M"_atom;
        if (key == Key_N) return "N"_atom;
        if (key == Key_O) return "O"_atom;
        if (key == Key_P) return "P"_atom;
        if (key == Key_Q) return "Q"_atom;
        if (key == Key_R) return "R"_atom;
        if (key == Key_S) return "S"_atom;
        if (key == Key_T) return "T"_atom;
        if (key == Key_U) return "U"_atom;
        if (key == Key_V) return "V"_atom;
        if (key == Key_W) return "W"_atom;
        if (key == Key_X) return "X"_atom;
        if (key == Key_Y) return "Y"_atom;
        if (key == Key_Z) return "Z"_atom;
        if (key == Key_Left_Bracket) return "Left_Bracket"_atom;
        if (key == Key_Backslash) return "Backslash"_atom;
        if (key == Key_Right_Bracket) return "Right_Bracket"_atom;
        if (key == Key_Grave_Accent) return "Grave_Accent"_atom;
        if (key == Key_World_1) return "World_1"_atom;
        if (key == Key_World_2) return "World_2"_atom;
        if (key == Key_Escape) return "Escape"_atom;
        if (key == Key_Enter) return "Enter"_atom;
        if (key == Key_Tab) return "Tab"_atom;
        if (key == Key_Backspace) return "Backspace"_atom;
        if (key == Key_Insert) return "Insert"_atom;
        if (key == Key_Delete) return "Delete"_atom;
        if (key == Key_Right) return "Right"_atom;
        if (key == Key_Left) return "Left"_atom;
        if (key == Key_Down) return "Down"_atom;
        if (key == Key_Up) return "Up"_atom;
        if (key == Key_Page_Up) return "Page_Up"_atom;
        if (key == Key_Page_Down) return "Page_Down"_atom;
        if (key == Key_Home) return "Home"_atom;
        if (key == Key_End) return "End"_atom;
        if (key == Key_Caps_Lock) return "Caps_Lock"_atom;
        if (key == Key_Scroll_Lock) return "Scroll_Lock"_atom;
        if (key == Key_Num_Lock) return "Num_Lock"_atom;
        if (key == Key_Print_Screen) return "Print_Screen"_atom;
        if (key == Key_Pause) return "Pause"_atom;
        if (key == Key_F1) return "F1"_atom;
        if (key == Key_F2) return "F2"_atom;
        if (key == Key_F3) return "F3"_atom;
        if (key == Key_F4) return "F4"_atom;
        if (key == Key_F5) return "F5"_atom;
        if (key == Key_F6) return "F6"_atom;
        if (key == Key_F7) return "F7"_atom;
        if (key == Key_F8) return "F8"_atom;
        if (key == Key_F9) return "F9"_atom;
        if (key == Key_F10) return "F10"_atom;
        if (key == Key_F11) return "F11"_atom;
        if (key == Key_F12) return "F12"_atom;
        if (key == Key_F13) return "F13"_atom;
        if (key == Key_F14) return "F14"_atom;
        if (key == Key_F15) return "F15"_atom;
        if (key == Key_F16) return "F16"_atom;
        if (key == Key_F17) return "F17"_atom;
        if (key == Key_F18) return "F18"_atom;
        if (key == Key_F19) return "F19"_atom;
        if (key == Key_F20) return "F20"_atom;
        if (key == Key_F21) return "F21"_atom;
        if (key == Key_F22) return "F22"_atom;
        if (key == Key_F23) return "F23"_atom;
        if (key == Key_F24) return "F24"_atom;
        if (key == Key_F25) return "F25"_atom;
        if (key == Key_Kp_0) return "Kp_0"_atom;
        if (key == Key_Kp_1) return "Kp_1"_atom;
        if (key == Key_Kp_2) return "Kp_2"_atom;
        if (key == Key_Kp_3) return "Kp_3"_atom;
        if (key == Key_Kp_4) return "Kp_4"_atom;
        if (key == Key_Kp_5) return "Kp_5"_atom;
        if (key == Key_Kp_6) return "Kp_6"_atom;
        if (key == Key_Kp_7) return "Kp_7"_atom;
        if (key == Key_Kp_8) return "Kp_8"_atom;
        if (key == Key_Kp_9) return "Kp_9"_atom;
        if (key == Key_Kp_Decimal) return "Kp_Decimal"_atom;
        if (key == Key_Kp_Divide) return "Kp_Divide"_atom;
        if (key == Key_Kp_Multiply) return "Kp_Multiply"_atom;
        if (key == Key_Kp_Subtract) return "Kp_Subtract"_atom;
        if (key == Key_Kp_Add) return "Kp_Add"_atom;
        if (key == Key_Kp_Enter) return "Kp_Enter"_atom;
        if (key == Key_Kp_Equal) return "Kp_Equal"_atom;
        if (key == Key_Left_Shift) return "Left_Shift"_atom;
        if (key == Key_Left_Control) return "Left_Control"_atom;
        if (key == Key_Left_Alt) return "Left_Alt"_atom;
        if (key == Key_Left_Super) return "Left_Super"_atom;
        if (key == Key_Right_Shift) return "Right_Shift"_atom;
        if (key == Key_Right_Control) return "Right_Control"_atom;
        if (key == Key_Right_Alt) return "Right_Alt"_atom;
        if (key == Key_Right_Super) return "Right_Super"_atom;
        if (key == Key_Menu) return "Menu"_atom;
        if (key == Key_Last) return "Last"_atom;
        // clang-format on

        return "None"_atom;
    }

    Keyboard::Key Keyboard::FromStringToKey(const StringAtom& str)
    {
        // clang-format off
        if (str == "None"_atom) return Key_None;
        if (str == "Space"_atom) return Key_Space;
        if (str == "Apostrophe"_atom) return Key_Apostrophe;
        if (str == "Comma"_atom) return Key_Comma;
        if (str == "Minus"_atom) return Key_Minus;
        if (str == "Period"_atom) return Key_Period;
        if (str == "Slash"_atom) return Key_Slash;
        if (str == "0"_atom) return Key_0;
        if (str == "1"_atom) return Key_1;
        if (str == "2"_atom) return Key_2;
        if (str == "3"_atom) return Key_3;
        if (str == "4"_atom) return Key_4;
        if (str == "5"_atom) return Key_5;
        if (str == "6"_atom) return Key_6;
        if (str == "7"_atom) return Key_7;
        if (str == "8"_atom) return Key_8;
        if (str == "9"_atom) return Key_9;
        if (str == "Semicolon"_atom) return Key_Semicolon;
        if (str == "Equal"_atom) return Key_Equal;
        if (str == "A"_atom) return Key_A;
        if (str == "B"_atom) return Key_B;
        if (str == "C"_atom) return Key_C;
        if (str == "D"_atom) return Key_D;
        if (str == "E"_atom) return Key_E;
        if (str == "F"_atom) return Key_F;
        if (str == "G"_atom) return Key_G;
        if (str == "H"_atom) return Key_H;
        if (str == "I"_atom) return Key_I;
        if (str == "J"_atom) return Key_J;
        if (str == "K"_atom) return Key_K;
        if (str == "L"_atom) return Key_L;
        if (str == "M"_atom) return Key_M;
        if (str == "N"_atom) return Key_N;
        if (str == "O"_atom) return Key_O;
        if (str == "P"_atom) return Key_P;
        if (str == "Q"_atom) return Key_Q;
        if (str == "R"_atom) return Key_R;
        if (str == "S"_atom) return Key_S;
        if (str == "T"_atom) return Key_T;
        if (str == "U"_atom) return Key_U;
        if (str == "V"_atom) return Key_V;
        if (str == "W"_atom) return Key_W;
        if (str == "X"_atom) return Key_X;
        if (str == "Y"_atom) return Key_Y;
        if (str == "Z"_atom) return Key_Z;
        if (str == "Left_Bracket"_atom) return Key_Left_Bracket;
        if (str == "Backslash"_atom) return Key_Backslash;
        if (str == "Right_Bracket"_atom) return Key_Right_Bracket;
        if (str == "Grave_Accent"_atom) return Key_Grave_Accent;
        if (str == "World_1"_atom) return Key_World_1;
        if (str == "World_2"_atom) return Key_World_2;
        if (str == "Escape"_atom) return Key_Escape;
        if (str == "Enter"_atom) return Key_Enter;
        if (str == "Tab"_atom) return Key_Tab;
        if (str == "Backspace"_atom) return Key_Backspace;
        if (str == "Insert"_atom) return Key_Insert;
        if (str == "Delete"_atom) return Key_Delete;
        if (str == "Right"_atom) return Key_Right;
        if (str == "Left"_atom) return Key_Left;
        if (str == "Down"_atom) return Key_Down;
        if (str == "Up"_atom) return Key_Up;
        if (str == "Page_Up"_atom) return Key_Page_Up;
        if (str == "Page_Down"_atom) return Key_Page_Down;
        if (str == "Home"_atom) return Key_Home;
        if (str == "End"_atom) return Key_End;
        if (str == "Caps_Lock"_atom) return Key_Caps_Lock;
        if (str == "Scroll_Lock"_atom) return Key_Scroll_Lock;
        if (str == "Num_Lock"_atom) return Key_Num_Lock;
        if (str == "Print_Screen"_atom) return Key_Print_Screen;
        if (str == "Pause"_atom) return Key_Pause;
        if (str == "F1"_atom) return Key_F1;
        if (str == "F2"_atom) return Key_F2;
        if (str == "F3"_atom) return Key_F3;
        if (str == "F4"_atom) return Key_F4;
        if (str == "F5"_atom) return Key_F5;
        if (str == "F6"_atom) return Key_F6;
        if (str == "F7"_atom) return Key_F7;
        if (str == "F8"_atom) return Key_F8;
        if (str == "F9"_atom) return Key_F9;
        if (str == "F10"_atom) return Key_F10;
        if (str == "F11"_atom) return Key_F11;
        if (str == "F12"_atom) return Key_F12;
        if (str == "F13"_atom) return Key_F13;
        if (str == "F14"_atom) return Key_F14;
        if (str == "F15"_atom) return Key_F15;
        if (str == "F16"_atom) return Key_F16;
        if (str == "F17"_atom) return Key_F17;
        if (str == "F18"_atom) return Key_F18;
        if (str == "F19"_atom) return Key_F19;
        if (str == "F20"_atom) return Key_F20;
        if (str == "F21"_atom) return Key_F21;
        if (str == "F22"_atom) return Key_F22;
        if (str == "F23"_atom) return Key_F23;
        if (str == "F24"_atom) return Key_F24;
        if (str == "F25"_atom) return Key_F25;
        if (str == "Kp_0"_atom) return Key_Kp_0;
        if (str == "Kp_1"_atom) return Key_Kp_1;
        if (str == "Kp_2"_atom) return Key_Kp_2;
        if (str == "Kp_3"_atom) return Key_Kp_3;
        if (str == "Kp_4"_atom) return Key_Kp_4;
        if (str == "Kp_5"_atom) return Key_Kp_5;
        if (str == "Kp_6"_atom) return Key_Kp_6;
        if (str == "Kp_7"_atom) return Key_Kp_7;
        if (str == "Kp_8"_atom) return Key_Kp_8;
        if (str == "Kp_9"_atom) return Key_Kp_9;
        if (str == "Kp_Decimal"_atom) return Key_Kp_Decimal;
        if (str == "Kp_Divide"_atom) return Key_Kp_Divide;
        if (str == "Kp_Multiply"_atom) return Key_Kp_Multiply;
        if (str == "Kp_Subtract"_atom) return Key_Kp_Subtract;
        if (str == "Kp_Add"_atom) return Key_Kp_Add;
        if (str == "Kp_Enter"_atom) return Key_Kp_Enter;
        if (str == "Kp_Equal"_atom) return Key_Kp_Equal;
        if (str == "Left_Shift"_atom) return Key_Left_Shift;
        if (str == "Left_Control"_atom) return Key_Left_Control;
        if (str == "Left_Alt"_atom) return Key_Left_Alt;
        if (str == "Left_Super"_atom) return Key_Left_Super;
        if (str == "Right_Shift"_atom) return Key_Right_Shift;
        if (str == "Right_Control"_atom) return Key_Right_Control;
        if (str == "Right_Alt"_atom) return Key_Right_Alt;
        if (str == "Right_Super"_atom) return Key_Right_Super;
        if (str == "Menu"_atom) return Key_Menu;
        if (str == "Last"_atom) return Key_Last;
        // clang-format on

        return Key_None;
    }

    Keyboard::KeyState Keyboard::getKeyState(Key key)
    {
        return glfwGetKey(GetWindow().getRawWindow(), key);
    }

    bool Keyboard::IsKeyPressed(Key key)
    {
        return glfwGetKey(GetWindow().getRawWindow(), key) == GLFW_PRESS;
    }

    bool Keyboard::isKeyRepeated(Key key)
    {
        return glfwGetKey(GetWindow().getRawWindow(), key) == GLFW_REPEAT;
    }

    bool Keyboard::IsKeyReleased(Key key)
    {
        return glfwGetKey(GetWindow().getRawWindow(), key) == GLFW_RELEASE;
    }

} // namespace Core
