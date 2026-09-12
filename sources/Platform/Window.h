/*
 * MIT License
 *
 * Copyright (c) 2018-2027 Valerii Koniushenko
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
#include "Core/Singleton.h"
#include "Core/Size.h"
#include "Foundation/BaseLog.h"
#include "Foundation/Interfaces/DataStream.h"
#include "Glfw.h"
#include "Keyboard.h"
#include "Mouse.h"

namespace Platform
{
    struct DragAndDrop
    {
        struct Data
        {
            Data() = default;
            Data(const Data&) = default;
            Data(Data&&) noexcept = default;
            Data& operator=(const Data&) = default;
            Data& operator=(Data&&) noexcept = default;
            virtual ~Data() = default;
        };

        struct Payload
        {
            std::unique_ptr<Data> data;
            Core::StringAtom type;
        };

        ENUM_CLASS();
        enum class State : std::uint8_t
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

    CLASS();
    // NOLINTNEXTLINE(misc-multiple-inheritance): combines logging, singleton access, and data I/O.
    class Window :
        public Foundation::BaseLog,
        public Core::Singleton<Window>,
        public Foundation::IDataIO
    {
        SINGLETONS_FRIEND(Window);
        R_FRIEND(Window);

    public:
        ENUM_CLASS();
        enum class CursorMode
        {
            Normal = GLFW_CURSOR_NORMAL,
            Disabled = GLFW_CURSOR_DISABLED,
            Hidden = GLFW_CURSOR_HIDDEN
        };

    public:
        ~Window() override;

        void create(const Core::StringAtom& title, Core::ISize2 size = Core::ISize2{ 300, 300 });

        void close();

        void destroy();

        void clear(int code);

        [[nodiscard]] bool shouldClose() const;

        void swapBuffers();

        static void pollEvent();

        [[nodiscard]] Core::ISize2 getSize() const;

        void setCursorPosition(float x, float y);

        void setCursorPosition(glm::vec2 position);

        static void setCursorMode(CursorMode mode);

        static CursorMode getCursorMode();

        void toggleCursorMode();

        [[nodiscard]] GLFWwindow* getRawWindow() noexcept { return _window; }

        [[nodiscard]] Core::StringAtom getCacheHash() const override;
        [[nodiscard]] spdlog::logger* getLogger() const override;
        [[nodiscard]] const char* getPrefix() const override { return "Window"; }

    public:
        /// @param glm::vec2 mouse position (X & Y)
        Core::Delegate<void(glm::vec2)>::Ptr onMouseMove
            = Core::Delegate<void(glm::vec2)>::Create();

        /// @param Mouse::Key is a key
        /// @param Mouse::State pressed or released
        /// @param Mouse::Mod is a mod
        Core::Delegate<void(Mouse::Key, Mouse::State, Mouse::Mod)>::Ptr onMouseKeyPressed
            = Core::Delegate<void(Mouse::Key, Mouse::State, Mouse::Mod)>::Create();

        /// @param glm::vec2 mouse scroll offsets (X & Y)
        Core::Delegate<void(glm::vec2)>::Ptr onMouseWheel
            = Core::Delegate<void(glm::vec2)>::Create();

        /// @param Keyboard::Key is a key
        /// @param int is a scancode
        /// @param Keyboard::KeyState is an action: GLFW_PRESS, GLFW_RELEASE or GLFW_REPEAT
        /// @param int is a mod
        Core::Delegate<void(Keyboard::Key, int, Keyboard::KeyState, int)>::Ptr onKeyPressed
            = Core::Delegate<void(Keyboard::Key, int, Keyboard::KeyState, int)>::Create();

        /// @param unsigned int is a Scancode
        Core::Delegate<void(unsigned int)>::Ptr onTextInput
            = Core::Delegate<void(unsigned int)>::Create();

        /// @param bool is Entered the cursor or no
        Core::Delegate<void(bool)>::Ptr onCursorEntered = Core::Delegate<void(bool)>::Create();

        /// @param ISize2 new window size
        Core::Delegate<void(Core::ISize2)>::Ptr onResize
            = Core::Delegate<void(Core::ISize2)>::Create();

    protected:
        Core::DelegateSubscriberPoolGuard _subscriptionPool;
        GLFWwindow* _window{};

        FIELD();
        Core::ISize2 _size;

        FIELD();
        Core::StringAtom _title;

        FIELD();
        bool _isMaximized = false;

        FIELD();
        bool _swapInterval = true;

    private:
        void registerEvents();
    };

    Window& GetWindow();
} // namespace Platform

#include "Window.generated.h" // added by the code generator. Better don't move it.
