// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "Window.h"

#include "Foundation/Configs.h"
#include "Platform/Graphics/Graphics.h"
#include "Platform/PrivateModuleInfo.h"
#include "Stb/Image.h"

#include <cstdlib>

#ifdef _WIN32
    #include "Platform/Resources/Resources.h"

    #include <windows.h>
#endif

using namespace Platform;
using namespace Core;

namespace
{
    void glfwErrorCallback(int error, const char* description)
    {
        gGlobalLog.errorLog("GLFW Error [{}]: {}"_f << error << description);
    }

    void MouseKeyPressHandler(GLFWwindow* window, int button, int action, int mods)
    {
        NX::MouseButtonEvent event;

        double x = 0.0;
        double y = 0.0;
        glfwGetCursorPos(window, &x, &y);

        event.setButton(NX::InputButton(static_cast<uint16_t>(button)));
        event.setState(ToInputState(action));
        event.setMods(ToInputModifiers(mods));
        event.setPosition({ static_cast<float>(x), static_cast<float>(y) });
        GetWindow().onMouseKeyPressed->trigger(event);
    }

    void KeyPressHandler(GLFWwindow*, int key, int scancode, int action, int mods)
    {
        NX::KeyboardEvent event;

        event.setKey(NX::InputButton(static_cast<uint16_t>(key)));
        event.setScancode(scancode);
        event.setState(ToInputState(action));
        event.setMods(ToInputModifiers(mods));

        GetWindow().onKeyPressed->trigger(event);
    }

    void TextInputHandler(GLFWwindow*, unsigned int scancode)
    {
        NX::TextInputEvent event;
        event.setCodepoint(scancode);
        GetWindow().onTextInput->trigger(event);
    }

    void CursorEnterHandler(GLFWwindow*, int entered)
    {
        NX::MouseEnterEvent event;
        event.setEntered(entered == GLFW_TRUE);

        GetWindow().onCursorEntered->trigger(event);
    }

    void MouseScrollHandler(GLFWwindow*, double x, double y)
    {
        NX::MouseScrollEvent event;
        event.setOffset({ static_cast<float>(x), static_cast<float>(y) });
        GetWindow().onMouseWheel->trigger(event);
    }

    void WindowFocusCallback(GLFWwindow*, int focused)
    {
        NX::WindowFocusEvent event;
        event.setFocused(focused == GLFW_TRUE);
        GetWindow().onFocusChanged->trigger(event);
    }

    void WindowIconifyCallback(GLFWwindow*, int iconified)
    {
        NX::WindowIconifyEvent event;
        event.setIconified(iconified == GLFW_TRUE);
        GetWindow().onIconifyChanged->trigger(event);
    }

#ifndef _WIN32
    void SetWindowIcon(GLFWwindow* window)
    {
        const auto path = Foundation::Config::Path::data / "internal/icons/logo_192.png";
        GLFWimage icon{};
        int channels = 0;
        icon.pixels = stbi_load(path.string().c_str(), &icon.width, &icon.height, &channels, 4);
        if (!icon.pixels)
        {
            gGlobalLog.errorLog("Can't load application's icon: {}"_f << path.string());
            return;
        }

        glfwSetWindowIcon(window, 1, &icon);
        stbi_image_free(icon.pixels);
    }
#endif
} // namespace

namespace Platform
{
    DragAndDrop gDragDrop;

    Window::~Window()
    {
        destroy();
    }

    void Window::create(const StringAtom& title, ISize2 size /* = { 300, 300 }*/)
    {
        constexpr int minSize = 100;
        if (_size.width < minSize || _size.height < minSize)
        {
            _size = size;
            _title = title;
        }

        glfwSetErrorCallback(glfwErrorCallback);
        if (!glfwInit())
        {
            criticalLogAndThrow("Failed to initialize GLFW!");
        }
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        // CI runs through Xvfb's software GLX implementation, which supports OpenGL 4.5.
        const int contextMinor = std::getenv("NEXIUM_HEADLESS_GL") ? 5 : 6;
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, contextMinor);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_MAXIMIZED, _isMaximized ? GLFW_TRUE : GLFW_FALSE);
        glfwWindowHintString(GLFW_WAYLAND_APP_ID, Foundation::Config::applicationId);
        glfwWindowHintString(GLFW_X11_CLASS_NAME, Foundation::Config::applicationId);
        glfwWindowHintString(GLFW_X11_INSTANCE_NAME, Foundation::Config::applicationId);

        _window = glfwCreateWindow(_size.width, _size.height, title.c_str(), nullptr, nullptr);
        if (!_window)
        {
            destroy();
            criticalLogAndThrow("Failed to create GLFW window");
        }

#ifndef _WIN32
        if (glfwGetPlatform() != GLFW_PLATFORM_WAYLAND)
        {
            SetWindowIcon(_window);
        }
#endif

        debugLog("The window was created");

        glfwMakeContextCurrent(_window);
        if (_isMaximized)
        {
            glfwMaximizeWindow(_window);
        }

        if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
        {
            criticalLogAndThrow("Impossible to initialize GLAD.");
        }

#if defined(NEXIUM_DEBUG)
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); // fires on the offending call's stack frame
        glDebugMessageCallback(
            [](GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei, const GLchar* msg,
               const void*)
            {
                if (severity != GL_DEBUG_SEVERITY_NOTIFICATION)
                {
                    gGlobalLog.errorLog("[GL] {}"_f << msg);
                }
            },
            nullptr);
#endif
        glfwSetMouseButtonCallback(_window, MouseKeyPressHandler);
        glfwSetCursorPosCallback(_window, CursorPositionCallback);
        glfwSetKeyCallback(_window, KeyPressHandler);
        glfwSetCharCallback(_window, TextInputHandler);
        glfwSetCursorEnterCallback(_window, CursorEnterHandler);
        glfwSetScrollCallback(_window, MouseScrollHandler);
        glfwSetWindowSizeCallback(_window, WindowSizeCallback);
        glfwSetFramebufferSizeCallback(_window, FramebufferSizeCallback);
        glfwSetWindowFocusCallback(_window, WindowFocusCallback);
        glfwSetWindowIconifyCallback(_window, WindowIconifyCallback);
        _isFocused = glfwGetWindowAttrib(_window, GLFW_FOCUSED) == GLFW_TRUE;
        _isIconified = glfwGetWindowAttrib(_window, GLFW_ICONIFIED) == GLFW_TRUE;
        glfwSwapInterval(_swapInterval);

        debugLog("OpenGL version: {}"_f << reinterpret_cast<const char*>(glGetString(GL_VERSION)));
        debugLog("GLFW version: {}"_f
                 << reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION)));

        registerEvents();
        glfwGetWindowSize(_window, &_size.width, &_size.height);
        glfwGetFramebufferSize(_window, &_framebufferSize.width, &_framebufferSize.height);

#ifdef _WIN32
        HWND hwnd = glfwGetWin32Window(_window);
        if (hwnd)
        {
            if (HICON hIcon = (HICON)LoadImage(GetModuleHandle(nullptr),
                                               MAKEINTRESOURCE(IDI_ICON_SMALL), IMAGE_ICON, 0,
                                               0, // width, height — 0,0 uses the icon's actual size
                                               LR_DEFAULTSIZE | LR_SHARED))
            {
                SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
            }
            if (HICON hIcon = (HICON)LoadImage(GetModuleHandle(nullptr),
                                               MAKEINTRESOURCE(IDI_ICON_BIG), IMAGE_ICON, 0,
                                               0, // width, height — 0,0 uses the icon's actual size
                                               LR_DEFAULTSIZE | LR_SHARED))
            {
                SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
            }
            else
            {
                errorLog("Can't load application's icon due to internal reasons.");
            }
        }
        else
        {
            criticalLog("Can't get a window's HWND");
        }
#endif
    }

    void Window::close()
    {
        glfwSetWindowShouldClose(_window, GLFW_TRUE);
    }

    void Window::destroy()
    {
        _isFocused = false;
        _isIconified = false;
        _framebufferSize = {};
        glfwDestroyWindow(_window);
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

    void Window::updateViewport()
    {
        glViewport(0, 0, _framebufferSize.width, _framebufferSize.height);
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
        _cursorDelta.resetTo({ x, y });
    }

    void Window::setCursorPosition(glm::vec2 position)
    {
        setCursorPosition(position.x, position.y);
    }

    void Window::setCursorMode(CursorMode mode)
    {
        auto& window = GetWindow();
        glfwSetInputMode(window.getRawWindow(), GLFW_CURSOR, static_cast<int>(mode));
        window._cursorDelta.reset();
    }

    Window::CursorMode Window::getCursorMode()
    {
        return static_cast<Window::CursorMode>(
            glfwGetInputMode(GetWindow().getRawWindow(), GLFW_CURSOR));
    }

    void Window::toggleCursorMode()
    {
        setCursorMode(getCursorMode() == CursorMode::Normal ? CursorMode::Disabled
                                                            : CursorMode::Normal);
    }

    void Window::CursorPositionCallback(GLFWwindow*, double x, double y)
    {
        auto& window = GetWindow();
        const glm::vec2 position{ static_cast<float>(x), static_cast<float>(y) };

        NX::MouseMoveEvent event;
        event.setPosition(position);
        event.setDelta(window._cursorDelta.update(position));

        window.onMouseMove->trigger(event);
    }

    void Window::WindowSizeCallback(GLFWwindow*, int width, int height)
    {
        auto& window = GetWindow();
        window._size = ISize2(width, height);
        window.onResize->trigger(window._size);
    }

    void Window::FramebufferSizeCallback(GLFWwindow*, int width, int height)
    {
        auto& window = GetWindow();
        window._framebufferSize = ISize2(width, height);
        window.onFramebufferResize->trigger(window._framebufferSize);
    }

    void Window::registerEvents()
    {
        _subscriptionPool << onFocusChanged->subscribeAndGetID(
            [this](const NX::WindowFocusEvent& event)
            {
                _isFocused = event.isFocused();
                _cursorDelta.reset();
            });
        _subscriptionPool << onCursorEntered->subscribeAndGetID([this](const NX::MouseEnterEvent&)
                                                                { _cursorDelta.reset(); });
        _subscriptionPool << onIconifyChanged->subscribeAndGetID(
            [this](const NX::WindowIconifyEvent& event) { _isIconified = event.isIconified(); });

        _subscriptionPool << onMouseMove->subscribeAndGetID(
            [](const NX::MouseMoveEvent& event)
            {
                const glm::vec2 pos = event.getPosition();
                // The event delta already ignores jumps after focus, enter, or cursor-mode changes.
                gDragDrop._lastDelta = -event.getDelta();

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

        _subscriptionPool << onMouseKeyPressed->subscribeAndGetID(
            [](const NX::MouseButtonEvent& event)
            {
                if (event.getState() == NX::InputState::Released)
                {
                    gDragDrop._state = DragAndDrop::State::Idle;
                    gDragDrop.payload = {};
                    gDragDrop._key.reset();
                }
                else if (event.getState() == NX::InputState::Pressed)
                {
                    gDragDrop._state = DragAndDrop::State::Started;
                    gDragDrop._key = event.getButton();
                    gDragDrop.payload.type = ""_atom;
                    gDragDrop._startPos = event.getPosition();
                }
            });
    }

    spdlog::logger* Window::getLogger() const
    {
        return Platform::getLogger();
    }

    void Window::onPostDeserialize(Window*, const RLogsCollector&)
    {
        if (!_window)
        {
            return;
        }

        glfwSetWindowTitle(_window, _title.c_str());
        glfwRestoreWindow(_window);
        glfwSetWindowSize(_window, _size.width, _size.height);
        if (_isMaximized)
        {
            glfwMaximizeWindow(_window);
        }
        glfwSwapInterval(_swapInterval);
    }

    StringAtom Window::getCacheHash() const
    {
        return "RootWindow"_atom;
    }

    Window& GetWindow()
    {
        return Window::Instance();
    }

} // namespace Platform
