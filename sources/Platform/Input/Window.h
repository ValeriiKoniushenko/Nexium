// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

// Module: Nexium::Platform::Input

#pragma once

#include "Core/Delegate.h"
#include "Core/Singleton.h"
#include "Core/Size.h"
#include "Foundation/BaseLog.h"
#include "Foundation/Interfaces/DataStream.h"
#include "Glfw.h"
#include "NxSubsystems/Input/InputTypes.h"
#include "Platform/Input/Events/GamepadEvents.h"
#include "Platform/Input/Events/InputEvent.h"
#include "Platform/Input/Events/KeyboardEvents.h"
#include "Platform/Input/Events/MouseEvents.h"
#include "Platform/Input/Events/WindowEvents.h"
#include "Platform/Input/InputDevices/Keyboard.h"
#include "Platform/Input/InputDevices/Mouse.h"

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
        [[nodiscard]] NX::InputButton getKey() const noexcept { return _key; }

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
        NX::InputButton _key;
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

        void updateViewport();

        void clear(int code);

        [[nodiscard]] bool shouldClose() const;
        [[nodiscard]] bool isFocused() const noexcept { return _isFocused; }
        [[nodiscard]] bool isIconified() const noexcept { return _isIconified; }

        void swapBuffers();

        static void pollEvent();

        [[nodiscard]] Core::ISize2 getSize() const;
        [[nodiscard]] Core::ISize2 getFramebufferSize() const noexcept { return _framebufferSize; }

        void setCursorPosition(float x, float y);

        void setCursorPosition(glm::vec2 position);

        static void setCursorMode(CursorMode mode);

        static CursorMode getCursorMode();

        void toggleCursorMode();

        [[nodiscard]] GLFWwindow* getRawWindow() noexcept { return _window; }

        void onPostDeserialize(Window* v, const RLogsCollector& logs);

        [[nodiscard]] Core::StringAtom getCacheHash() const override;
        [[nodiscard]] spdlog::logger* getLogger() const override;
        [[nodiscard]] const char* getPrefix() const override { return "Window"; }

    public:
        /// @param glm::vec2 mouse position (X & Y)
        Core::Delegate<void(NX::MouseMoveEvent)>::Ptr onMouseMove
            = Core::Delegate<void(NX::MouseMoveEvent)>::Create();

        /// @param Mouse::Key is a key
        /// @param Mouse::State pressed or released
        /// @param Mouse::Mod is a mod
        Core::Delegate<void(NX::MouseButtonEvent)>::Ptr onMouseKeyPressed
            = Core::Delegate<void(NX::MouseButtonEvent)>::Create();

        /// @param glm::vec2 mouse scroll offsets (X & Y)
        Core::Delegate<void(NX::MouseScrollEvent)>::Ptr onMouseWheel
            = Core::Delegate<void(NX::MouseScrollEvent)>::Create();

        /// @param Keyboard::Key is a key
        /// @param int is a scancode
        /// @param Keyboard::KeyState is an action: GLFW_PRESS, GLFW_RELEASE or GLFW_REPEAT
        /// @param int is a mod
        Core::Delegate<void(NX::KeyboardEvent)>::Ptr onKeyPressed
            = Core::Delegate<void(NX::KeyboardEvent)>::Create();

        Core::Delegate<void(NX::GamepadConnectionEvent)>::Ptr onJoystickConnected
            = Core::Delegate<void(NX::GamepadConnectionEvent)>::Create();

        /// @param unsigned int is a Scancode
        Core::Delegate<void(NX::TextInputEvent)>::Ptr onTextInput
            = Core::Delegate<void(NX::TextInputEvent)>::Create();

        /// @param bool is Entered the cursor or no
        Core::Delegate<void(NX::MouseEnterEvent)>::Ptr onCursorEntered
            = Core::Delegate<void(NX::MouseEnterEvent)>::Create();

        Core::Delegate<void(NX::WindowFocusEvent)>::Ptr onFocusChanged
            = Core::Delegate<void(NX::WindowFocusEvent)>::Create();

        Core::Delegate<void(NX::WindowIconifyEvent)>::Ptr onIconifyChanged
            = Core::Delegate<void(NX::WindowIconifyEvent)>::Create();

        /// @param ISize2 new window size
        Core::Delegate<void(Core::ISize2)>::Ptr onResize
            = Core::Delegate<void(Core::ISize2)>::Create();

        /// @param ISize2 new framebuffer size in pixels, which can differ from the window size.
        Core::Delegate<void(Core::ISize2)>::Ptr onFramebufferResize
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
        static void WindowSizeCallback(GLFWwindow*, int width, int height);
        static void FramebufferSizeCallback(GLFWwindow*, int width, int height);
        void registerEvents();
        Core::ISize2 _framebufferSize{};
        bool _isFocused = false;
        bool _isIconified = false;
    };

    Window& GetWindow();
} // namespace Platform

#include "Window.generated.h" // added by the code generator. Better don't move it.
