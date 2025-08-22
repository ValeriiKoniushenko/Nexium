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

#pragma once

#include "Core/Delegate.h"
#include "Core/Enum.h"
#include "Core/Singleton.h"
#include "Core/Size.h"
#include "InputDevices/Keyboard.h"
#include "InputDevices/Mouse.h"
#include "ModuleInfo.h"
#include "OpenGL.h"
#include "Misc/JsonCacheable.h"

#include <filesystem>

namespace Core
{

    struct DragAndDrop
    {
        struct Payload
        {
            void* data = nullptr;
            StringAtom type;
        };

        enum class State
        {
            Idle,
            Started,
            Dragging
        };

        constexpr static float dragTreshold = 25.f;

    public:
        Payload payload;

        [[nodiscard]] glm::vec2 getStartPos() const noexcept { return _startPos; };
        [[nodiscard]] glm::vec2 getCurrentPos() const noexcept { return _currentPos; };
        [[nodiscard]] State getState() const noexcept { return _state; };

    private:
        glm::vec2 _startPos;
        glm::vec2 _currentPos;
        State _state = State::Idle;

        friend class Window;
    };

    extern DragAndDrop gDragDrop;

    class Window : public BaseLog, public JsonCacheable, public JsonAdapter, public StrictSingleton<Window>
    {
    public:
        // clang-format off
        CreateEnum(CursorMode, int,
            Normal = GLFW_CURSOR_NORMAL,
            Disabled = GLFW_CURSOR_DISABLED,
            Hidden = GLFW_CURSOR_HIDDEN
        );
        // clang-format on

    public:
        Window() = default;
        ~Window() override;

        void create(const StringAtom& title, ISize2 size = ISize2{ 300, 300 });
        void close();
        void destroy();
        void clear(int code);

        [[nodiscard]] bool shouldClose() const;

        void swapBuffers();

        void pollEvent();

        void updateViewport(float aspect, ViewportMode mode = ViewportMode::Default);

        [[nodiscard]] ISize2 getSize() const;

        void setCursorPosition(float x, float y);
        void setCursorPosition(glm::vec2 position);

        void setCursorMode(CursorMode mode);
        CursorMode getCursorMode();
        void toggleCursorMode();

        [[nodiscard]] GLFWwindow* getRawWindow() noexcept { return _window; }

        /**
         * @param glm::vec2 mouse position (X & Y)
         */
        Delegate<void(glm::vec2)> onMouseMove;

        /**
         * @param Mouse::Key is a key
         * @param Mouse::State pressed or released
         * @param Mouse::Mod is a mod
         */
        Delegate<void(Mouse::Key, Mouse::State, Mouse::Mod)> onMouseKeyPressed;

        /**
         * @param glm::vec2 mouse scroll offsets (X & Y)
         */
        Delegate<void(glm::vec2)> onMouseWheel;

        /**
         * @param Keyboard::Key is a key
         * @param int is a scancode
         * @param Keyboard::KeyState is an action: GLFW_PRESS, GLFW_RELEASE or GLFW_REPEAT
         * @param int is a mod
         */
        Delegate<void(Keyboard::Key, int, Keyboard::KeyState, int)> onKeyPressed;

        /**
         * @param unsigned int is a Scancode
         */
        Delegate<void(unsigned int)> onTextInput;

        /**
         * @param bool is Entered the cursor or no
         */
        Delegate<void(bool)> onCursorEntered;

        /**
         * @param ISize2 new window size
         */
        Delegate<void(ISize2)> onResize;

        [[nodiscard]] spdlog::logger* getLogger() const override { return Graphics::getLogger(); }
        [[nodiscard]] const char* getPrefix() const override { return "Window"; }

        nlohmann::json toJson() const override;
        void fromJson(const nlohmann::json& json, bool isIgnoreChildren) override;

    protected:
        StringAtom getCacheHash() const override;
        nlohmann::json toCacheData() const override;
        void fromCacheData(const nlohmann::json& json) override;

    protected:
        GLFWwindow* _window{};
        ISize2 _size{};
        StringAtom _title;
        bool _isMaximized = false;
        bool _swapInterval = false;

    private:
        void registerEvents();

    private:
        decltype(onResize)::IDGuard _idOnResize;
        decltype(onMouseMove)::IDGuard _idOnMouseMove;
        decltype(onMouseKeyPressed)::IDGuard _idOnMouseKeyPressed;
    };

    Window& GetWindow();

} // namespace Core