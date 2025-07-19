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

#include "Mouse.h"

#include <stdexcept>
#include <utility>

namespace SW
{

    glm::vec2 Mouse::getPosition()
    {
        double x = 0, y = 0;
        glfwGetCursorPos(GetWindow().getRawWindow(), &x, &y);
        return { static_cast<float>(x), static_cast<float>(y) };
    }

    bool Mouse::isKeyPressed(int key)
    {
        return glfwGetMouseButton(GetWindow().getRawWindow(), key) == GLFW_PRESS;
    }

    bool Mouse::isKeyReleased(int key)
    {
        return glfwGetMouseButton(GetWindow().getRawWindow(), key) == GLFW_RELEASE;
    }

    bool Mouse::isKeyRepeated(int key)
    {
        return glfwGetMouseButton(GetWindow().getRawWindow(), key) == GLFW_REPEAT;
    }

    void Mouse::setCursorMode(CursorMode mode)
    {
        glfwSetInputMode(GetWindow().getRawWindow(), GLFW_CURSOR, mode.cast());
    }

    Mouse::CursorMode Mouse::getCursorMode()
    {
        return static_cast<CursorMode>(glfwGetInputMode(GetWindow().getRawWindow(), GLFW_CURSOR));
    }

} // namespace SW