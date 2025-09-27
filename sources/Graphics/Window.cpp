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

#include "Window.h"

#include "ImGui/imgui.h"

using namespace Core;

namespace
{
    void MouseMoveHandler(GLFWwindow*, double x, double y)
    {
        GetWindow().onMouseMove.trigger(glm::vec2(static_cast<float>(x), static_cast<float>(y)));
    }

    void MouseKeyPressHandler(GLFWwindow*, int button, int action, int mods)
    {
        GetWindow().onMouseKeyPressed.trigger(static_cast<Mouse::Key>(button),
                                              static_cast<Mouse::State>(action),
                                              static_cast<Mouse::Mod>(mods));
    }

    void KeyPressHandler(auto*, int key, int scancode, int action, int mods)
    {
        GetWindow().onKeyPressed.trigger(static_cast<Keyboard::Key>(key), scancode,
                                         static_cast<Keyboard::KeyState::UnderlyingType>(action),
                                         mods);
    }

    void TextInputHandler(auto*, unsigned int scancode)
    {
        GetWindow().onTextInput.trigger(scancode);
    }

    void CursorEnterHandler(auto*, int entered)
    {
        GetWindow().onCursorEntered.trigger(entered == GLFW_TRUE);
    }

    void MouseScrollHandler(auto*, double x, double y)
    {
        GetWindow().onMouseWheel.trigger(glm::vec2(static_cast<float>(x), static_cast<float>(y)));
    }

    void WindowSizeCallback(auto*, int width, int height)
    {
        GetWindow().onResize.trigger(ISize2(width, height));
    }
} // namespace

namespace Core
{
    DragAndDrop gDragDrop;

    Window::~Window()
    {
        destroy();
    }

    void Window::create(const StringAtom& title, ISize2 size /* = { 300, 300 }*/)
    {
        tryReadFromCache();

        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_MAXIMIZED, _isMaximized);

        if (!hasCache())
        {
            _size = size;
            _title = title;
        }

        if (!((_window
               = glfwCreateWindow(_size.width, _size.height, title.c_str(), nullptr, nullptr))))
        {
            glfwTerminate();
            criticalThrowingLog("Failed to create GLFW window");
        }

        debugLog("The window was created");

        glfwMakeContextCurrent(_window);
        glfwSetMouseButtonCallback(_window, MouseKeyPressHandler);
        glfwSetCursorPosCallback(_window, MouseMoveHandler);
        glfwSetKeyCallback(_window, KeyPressHandler);
        glfwSetCharCallback(_window, TextInputHandler);
        glfwSetCursorEnterCallback(_window, CursorEnterHandler);
        glfwSetScrollCallback(_window, MouseScrollHandler);
        glfwSetWindowSizeCallback(_window, WindowSizeCallback);
        glfwSwapInterval(_swapInterval);

        if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
        {
            criticalThrowingLog("Impossible to initialize GLAD.");
        }

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        debugLog("OpenGL version: {}"_f << reinterpret_cast<const char*>(glGetString(GL_VERSION)));
        debugLog("GLFW version: {}"_f
                 << reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION)));

        registerEvents();
    }

    void Window::close()
    {
        glfwSetWindowShouldClose(_window, GLFW_TRUE);
    }

    void Window::destroy()
    {
        writeToCache();
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
        return glfwGetInputMode(GetWindow().getRawWindow(), GLFW_CURSOR);
    }

    void Window::toggleCursorMode()
    {
        setCursorMode(getCursorMode().cast() == CursorMode::Normal ? CursorMode::Disabled
                                                                   : CursorMode::Normal);
    }

    void Window::registerEvents()
    {
        onResize.subscribe(
            [this](ISize2 size)
            {
                _size = size;
            });

        onMouseMove.subscribe(
            [](glm::vec2 pos)
            {
                static glm::vec2 lastPos = pos;
                gDragDrop._lastDelta = lastPos - pos;
                lastPos = pos;

                if (gDragDrop._state == DragAndDrop::State::Started)
                {
                    if (glm::distance(pos, gDragDrop._startPos) >= DragAndDrop::dragTreshold)
                    {
                        gDragDrop._state = DragAndDrop::State::Dragging;
                    }
                }
                if (gDragDrop._state == DragAndDrop::State::Dragging)
                {
                    gDragDrop._currentPos = pos;
                }
            });

        onMouseKeyPressed.subscribe(
            [](Mouse::Key key, Mouse::State state, Mouse::Mod mod)
            {
                if (state == Mouse::State::Release)
                {
                    if (glm::distance(gDragDrop._startPos, Mouse::GetPosition())
                        >= DragAndDrop::dragTreshold)
                    {
                        gDragDrop._state = DragAndDrop::State::Dragging;
                    }
                    gDragDrop._state = DragAndDrop::State::Idle;
                    gDragDrop.payload = {};
                    gDragDrop._key = Mouse::Key_None;
                }
                else if (state == Mouse::State::Press)
                {
                    gDragDrop._state = DragAndDrop::State::Started;
                    gDragDrop._key = key;
                    gDragDrop.payload.type = ""_atom;
                    gDragDrop._startPos = Mouse::GetPosition();
                }
            });
    }

    StringAtom Window::getCacheHash() const
    {
        return "RootWindow"_atom;
    }

    nlohmann::json Window::toCacheData() const
    {
        return toJson();
    }

    void Window::fromCacheData(const nlohmann::json& json)
    {
        fromJson(json, false);
    }

    nlohmann::json Window::toJson() const
    {
        nlohmann::json json;

        json["size"] = _size;
        json["title"] = _title;

        if (_window)
        {
            json["isMaximized"] = glfwGetWindowAttrib(_window, GLFW_MAXIMIZED) == GLFW_TRUE;
        }

        json["vSync"] = _swapInterval;

        return json;
    }

    void Window::fromJson(const nlohmann::json& json, bool isIgnoreChildren)
    {
        tryReadJsonTo(_size, "size", json);
        tryReadJsonTo(_title, "title", json);
        tryReadJsonTo(_isMaximized, "isMaximized", json);
        tryReadJsonTo(_swapInterval, "vSync", json);
    }

    Window& GetWindow()
    {
        return Window::instance();
    }
} // namespace Core