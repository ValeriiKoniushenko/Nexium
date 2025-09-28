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

#include "Mouse.h"

#include "Editor/Windows/GameViewport.h"
#include "GameplaySystem/Framework/GameInstance.h"
#include "Graphics/Window.h"

namespace Core
{
    StringAtom Mouse::KeyToString(Key key)
    {
        // clang-format off
        if (key == Key_Last) return "Last"_atom;
        if (key == Key_Left) return "Left"_atom;
        if (key == Key_Right) return "Right"_atom;
        if (key == Key_Middle) return "Middle"_atom;
        if (key == Key_1) return "1"_atom;
        if (key == Key_2) return "2"_atom;
        if (key == Key_3) return "3"_atom;
        if (key == Key_4) return "4"_atom;
        if (key == Key_5) return "5"_atom;
        if (key == Key_6) return "6"_atom;
        if (key == Key_7) return "7"_atom;
        if (key == Key_8) return "8"_atom;
        // clang-format on

        return "None"_atom;
    }

    Mouse::Key Mouse::FromStringToKey(const StringAtom& str)
    {
        // clang-format off
        if (str == "Last"_atom) return Key_Last;
        if (str == "Left"_atom) return Key_Left;
        if (str == "Right"_atom) return Key_Right;
        if (str == "Middle"_atom) return Key_Middle;
        if (str == "1"_atom) return Key_1;
        if (str == "2"_atom) return Key_2;
        if (str == "3"_atom) return Key_3;
        if (str == "4"_atom) return Key_4;
        if (str == "5"_atom) return Key_5;
        if (str == "6"_atom) return Key_6;
        if (str == "7"_atom) return Key_7;
        if (str == "8"_atom) return Key_8;
        // clang-format on

        return Key_None;
    }

    glm::vec2 Mouse::GetPosition()
    {
        double x = 0;
        double y = 0;
        glfwGetCursorPos(GetWindow().getRawWindow(), &x, &y);
        return { static_cast<float>(x), static_cast<float>(y) };
    }

    glm::vec2 Mouse::GetInViewportPosition()
    {
        if (gGameInstance->renderMode == GameInstance::RenderMode::Editor)
        {
            if (auto* wnd = gGameInstance->gameEditor.getWindow<GameViewportEWC>())
            {
                const auto wndPos = wnd->getInnerPosition();
                auto mouse = GetPosition();
                mouse.x -= wndPos.x;
                mouse.y -= wndPos.y;
                mouse.y = wnd->getInnerWindowSize().height - mouse.y - 1;

                return mouse;
            }
        }

        return GetPosition();
    }

    bool Mouse::IsKeyPressed(Key key)
    {
        return glfwGetMouseButton(GetWindow().getRawWindow(), key) == GLFW_PRESS;
    }

    bool Mouse::IsKeyReleased(Key key)
    {
        return glfwGetMouseButton(GetWindow().getRawWindow(), key) == GLFW_RELEASE;
    }

    bool Mouse::isKeyRepeated(Key key)
    {
        return glfwGetMouseButton(GetWindow().getRawWindow(), key) == GLFW_REPEAT;
    }
} // namespace Core