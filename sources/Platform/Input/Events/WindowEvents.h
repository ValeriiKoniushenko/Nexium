// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

// Module: Nexium::Subsystem::Input

#pragma once

#include "Core/Size.h"
#include "InputEvent.h"

namespace NX
{

    class WindowEvent : public InputEvent
    {
    protected:
        explicit WindowEvent(EventType eventType)
            : InputEvent(eventType, InputDeviceType::None)
        {
        }
    };

    class WindowResizeEvent : public WindowEvent
    {
    public:
        WindowResizeEvent()
            : WindowEvent(EventType::WindowResize)
        {
        }

        void setSize(const Core::ISize2& size) { _size = size; }
        void setFramebufferSize(const Core::ISize2& size) { _framebufferSize = size; }

        [[nodiscard]] const Core::ISize2& getSize() const { return _size; }
        [[nodiscard]] const Core::ISize2& getFramebufferSize() const { return _framebufferSize; }

    private:
        Core::ISize2 _size;
        Core::ISize2 _framebufferSize;
    };

    class WindowFocusEvent : public WindowEvent
    {
    public:
        WindowFocusEvent()
            : WindowEvent(EventType::WindowFocus)
        {
        }

        void setFocused(bool focused) { _focused = focused; }

        [[nodiscard]] bool isFocused() const { return _focused; }

    private:
        bool _focused = false;
    };

    class WindowIconifyEvent : public WindowEvent
    {
    public:
        WindowIconifyEvent()
            : WindowEvent(EventType::WindowIconify)
        {
        }

        void setIconified(bool iconified) { _iconified = iconified; }

        [[nodiscard]] bool isIconified() const { return _iconified; }

    private:
        bool _iconified = false;
    };

    class WindowCloseEvent : public WindowEvent
    {
    public:
        WindowCloseEvent()
            : WindowEvent(EventType::WindowClose)
        {
        }
    };

} // namespace NX