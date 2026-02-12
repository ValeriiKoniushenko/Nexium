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


#include "Mouse.generated.cpp.inl" // this line added by the code generator.

namespace Core
{

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
        return glfwGetMouseButton(GetWindow().getRawWindow(), static_cast<int>(key)) == GLFW_PRESS;
    }

    bool Mouse::IsKeyReleased(Key key)
    {
        return glfwGetMouseButton(GetWindow().getRawWindow(), static_cast<int>(key)) == GLFW_RELEASE;
    }

    bool Mouse::isKeyRepeated(Key key)
    {
        return glfwGetMouseButton(GetWindow().getRawWindow(), static_cast<int>(key)) == GLFW_REPEAT;
    }
} // namespace Core