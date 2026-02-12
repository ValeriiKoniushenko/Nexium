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

#include "Core/Delegate.h"
#include "Core/Enum.h"
#include "Core/Singleton.h"
#include "Core/Size.h"
#include "InputDevices/Keyboard.h"
#include "InputDevices/Mouse.h"
#include "Misc/BaseLog.h"
#include "Misc/DataStream.h"
#include "OpenGL.h"

#include <filesystem>

namespace Core
{
    struct DragAndDrop
    {
        struct Data
        {
            virtual ~Data() = default;
        };

        struct Payload
        {
            std::unique_ptr<Data> data;
            StringAtom type;
        };

        ENUM_CLASS;
        enum class State
        {
            Idle,
            Started,
            Dragging
        };

        constexpr static float dragTreshold = 15.f;

    public:
        Payload payload;

        [[nodiscard]] glm::vec2 getStartPos() const noexcept { return _startPos; }
        [[nodiscard]] glm::vec2 getCurrentPos() const noexcept { return _currentPos; }
        [[nodiscard]] glm::vec2 getLastDelta() const noexcept { return _lastDelta; }
        [[nodiscard]] State getState() const noexcept { return _state; }
        [[nodiscard]] Mouse::Key getKey() const noexcept { return _key; }

        template<class T>
            requires std::derived_from<std::remove_reference_t<T>, Data>
                     && requires() { T::dragType; }
        bool isTypeOf()
        {
            if (payload.type.isEmpty())
            {
                return false;
            }

            Assert(T::dragType.isStatic());
            Assert(payload.type.isStatic());
            return payload.type == T::dragType;
        }

    private:
        glm::vec2 _lastDelta = {};
        glm::vec2 _startPos = {};
        glm::vec2 _currentPos = {};
        Mouse::Key _key = Mouse::Key::None;
        State _state = State::Idle;

        friend class Window;
    };

    extern DragAndDrop gDragDrop;

    class Window : public BaseLog, public IDataStreamBridge, public Singleton<Window>
    {
    public:
        ENUM_CLASS;
        enum class CursorMode
        {
            Normal = GLFW_CURSOR_NORMAL,
            Disabled = GLFW_CURSOR_DISABLED,
            Hidden = GLFW_CURSOR_HIDDEN
        };

    public:
        Window() = default;

        ~Window() override;

        void create(const StringAtom& title, ISize2 size = ISize2{ 300, 300 });

        void close();

        void destroy();

        void clear(int code);

        [[nodiscard]] bool shouldClose() const;

        void swapBuffers();

        static void pollEvent();

        void updateViewport(float aspect, ViewportMode mode = ViewportMode::Default);

        [[nodiscard]] ISize2 getSize() const;

        void setCursorPosition(float x, float y);

        void setCursorPosition(glm::vec2 position);

        static void setCursorMode(CursorMode mode);

        static CursorMode getCursorMode();

        void toggleCursorMode();

        [[nodiscard]] GLFWwindow* getRawWindow() noexcept { return _window; }

        /**
         * @param glm::vec2 mouse position (X & Y)
         */
        Delegate<void(glm::vec2)>::Ptr onMouseMove = Delegate<void(glm::vec2)>::Create();

        /**
         * @param Mouse::Key is a key
         * @param Mouse::State pressed or released
         * @param Mouse::Mod is a mod
         */
        Delegate<void(Mouse::Key, Mouse::State, Mouse::Mod)>::Ptr onMouseKeyPressed
            = Delegate<void(Mouse::Key, Mouse::State, Mouse::Mod)>::Create();

        /**
         * @param glm::vec2 mouse scroll offsets (X & Y)
         */
        Delegate<void(glm::vec2)>::Ptr onMouseWheel = Delegate<void(glm::vec2)>::Create();

        /**
         * @param Keyboard::Key is a key
         * @param int is a scancode
         * @param Keyboard::KeyState is an action: GLFW_PRESS, GLFW_RELEASE or GLFW_REPEAT
         * @param int is a mod
         */
        Delegate<void(Keyboard::Key, int, Keyboard::KeyState, int)>::Ptr onKeyPressed
            = Delegate<void(Keyboard::Key, int, Keyboard::KeyState, int)>::Create();

        /**
         * @param unsigned int is a Scancode
         */
        Delegate<void(unsigned int)>::Ptr onTextInput = Delegate<void(unsigned int)>::Create();

        /**
         * @param bool is Entered the cursor or no
         */
        Delegate<void(bool)>::Ptr onCursorEntered = Delegate<void(bool)>::Create();

        /**
         * @param ISize2 new window size
         */
        Delegate<void(ISize2)>::Ptr onResize = Delegate<void(ISize2)>::Create();

        [[nodiscard]] spdlog::logger* getLogger() const override;
        [[nodiscard]] const char* getPrefix() const override { return "Window"; }

        void ioFieldsUpdate(DataStream& out) override;

    protected:
        StringAtom getCacheHash() const override;

    protected:
        DelegateSubscriberPoolGuard _subscriptionPool;
        GLFWwindow* _window{};
        ISize2 _size{};
        StringAtom _title;
        bool _isMaximized = false;
        bool _swapInterval = true;

    private:
        void registerEvents();
    };

    Window& GetWindow();
} // namespace Core
#include "Window.generated.h" // added by the code generator. Better don't move it.
