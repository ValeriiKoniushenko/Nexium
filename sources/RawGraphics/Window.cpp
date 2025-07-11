// MIT License
//
// Copyright (c) 2019-2025 Valerii Koniushenko
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

#include "Window.h"

namespace
{

    void KeyPressHandler(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        SW::GetWindow().onKeyPressed.trigger(key, scancode, action, mods);
    }

    void TextInputHandler(GLFWwindow* window, unsigned int scancode)
    {
        SW::GetWindow().onTextInput.trigger(scancode);
        // GetWorldVariables().set("inputted-text", scancode);
    }

    void CursorEnterHandler(GLFWwindow* window, int entered)
    {
        SW::GetWindow().onCursorEntered.trigger(entered == GLFW_TRUE);
    }

    void MouseScrollHandler(GLFWwindow* window, double x, double y)
    {
        SW::GetWindow().onMouseWheel.trigger(
            glm::vec2(static_cast<float>(x), static_cast<float>(y)));
        // GetWorldVariables().set("mouse-wheel-x", xOffset);
        // GetWorldVariables().set("mouse-wheel-y", yOffset);
    }

} // namespace

namespace SW
{

    Window::~Window()
    {
        glfwTerminate();
    }

    void Window::create(const Core::StringAtom& title, Core::ISize2 size /* = { 300, 300 }*/)
    {
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        if (!((_window
               = glfwCreateWindow(size.width, size.height, title.c_str(), nullptr, nullptr))))
        {
            glfwTerminate();
            criticalThrowingLog("Failed to create GLFW window");
        }
        _size = size;
        _title = title;

        debugLog("The window was created");

        glfwMakeContextCurrent(_window);
        glfwSetKeyCallback(_window, KeyPressHandler);
        glfwSetCharCallback(_window, TextInputHandler);
        glfwSetCursorEnterCallback(_window, CursorEnterHandler);
        glfwSetScrollCallback(_window, MouseScrollHandler);
        glfwSwapInterval(0);

        if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
        {
            criticalThrowingLog("Impossible to initialize GLAD.");
        }
    }

    bool Window::shouldClose() const
    {
        return glfwWindowShouldClose(_window);
    }

    void Window::swapBuffers()
    {
        glfwSwapBuffers(_window);
    }

    void Window::pollEvent()
    {
        glfwPollEvents();
    }

    void Window::clear(int code)
    {
        glClear(code);
    }

    void Window::viewport(GLint x, GLint y, GLsizei width, GLsizei height)
    {
        glViewport(x, y, width, height);
    }

    Core::ISize2 Window::getSize() const
    {
        return _size;
    }

    void Window::setCursorPosition(float x, float y)
    {
        glfwSetCursorPos(_window, x, y);
    }

    void Window::setCursorPosition(glm::vec2 position)
    {
        setCursorPosition(position.x, position.y);
    }

    Window& GetWindow()
    {
        return Window::instance();
    }

} // namespace SW