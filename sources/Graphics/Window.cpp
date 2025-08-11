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

#include "ImGui/imgui.h"

using namespace Core;

namespace
{

    void KeyPressHandler(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        GetWindow().onKeyPressed.trigger(key, scancode, action, mods);
    }

    void TextInputHandler(GLFWwindow* window, unsigned int scancode)
    {
        GetWindow().onTextInput.trigger(scancode);
    }

    void CursorEnterHandler(GLFWwindow* window, int entered)
    {
        GetWindow().onCursorEntered.trigger(entered == GLFW_TRUE);
    }

    void MouseScrollHandler(GLFWwindow* window, double x, double y)
    {
        GetWindow().onMouseWheel.trigger(glm::vec2(static_cast<float>(x), static_cast<float>(y)));
    }

    void WindowSizeCallback(GLFWwindow* window, int width, int height)
    {
        GetWindow().onResize.trigger(ISize2(width, height));
        GetWindow().m__setSize(ISize2(width, height));
    }

} // namespace

namespace Core
{

    Window::~Window()
    {
        destroy();
    }

    void Window::create(const StringAtom& title, ISize2 size /* = { 300, 300 }*/)
    {
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
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
        glfwSetWindowSizeCallback(_window, WindowSizeCallback);
        glfwSwapInterval(1);

        if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
        {
            criticalThrowingLog("Impossible to initialize GLAD.");
        }

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        debugLog("OpenGL version: {}"_f << reinterpret_cast<const char*>(glGetString(GL_VERSION)));
        debugLog("GLFW version: {}"_f
                 << reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION)));
    }

    void Window::close()
    {
        glfwSetWindowShouldClose(_window, GLFW_TRUE);
    }

    void Window::destroy()
    {
        ImGui::DestroyContext();
        glfwTerminate();
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

    void Window::updateViewport(float aspect, ViewportMode mode /* = ViewportMode::Default*/)
    {
        UpdateGlViewport(static_cast<FSize2>(_size), aspect, mode);
    }

    void Window::clear(int code)
    {
        glClear(code);
    }

    ISize2 Window::getSize() const
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

    void Window::setCursorMode(CursorMode mode)
    {
        glfwSetInputMode(GetWindow().getRawWindow(), GLFW_CURSOR, mode.cast());
    }

    Window::CursorMode Window::getCursorMode()
    {
        return static_cast<CursorMode>(glfwGetInputMode(GetWindow().getRawWindow(), GLFW_CURSOR));
    }

    void Window::toggleCursorMode()
    {
        setCursorMode(getCursorMode().cast() == CursorMode::Normal ? CursorMode::Disabled
                                                                   : CursorMode::Normal);
    }

    Window& GetWindow()
    {
        return Window::instance();
    }

} // namespace Core