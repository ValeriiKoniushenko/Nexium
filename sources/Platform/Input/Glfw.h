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

#define GLFW_INCLUDE_NONE
#ifdef _WIN32
    #define GLFW_EXPOSE_NATIVE_WIN32
#endif

#include "GLFW/glfw3.h"

#ifdef _WIN32
    #include "GLFW/glfw3native.h"
#endif

#include "Platform/Input/Events/InputEvent.h"

namespace Platform
{
    /// Converts a GLFW key/mouse-button action (GLFW_PRESS, GLFW_RELEASE, GLFW_REPEAT) to the
    /// platform-independent NX::InputState. Unknown values map to NX::InputState::None.
    [[nodiscard]] constexpr NX::InputState ToInputState(int glfwAction) noexcept
    {
        switch (glfwAction)
        {
            case GLFW_PRESS:
                return NX::InputState::Pressed;
            case GLFW_RELEASE:
                return NX::InputState::Released;
            case GLFW_REPEAT:
                return NX::InputState::Repeated;
            default:
                return NX::InputState::None;
        }
    }

    [[nodiscard]] constexpr NX::InputModifier ToInputModifiers(int glfwMods) noexcept
    {
        auto result = NX::InputModifier::None;

        if (glfwMods & GLFW_MOD_SHIFT)
        {
            result = result | NX::InputModifier::Shift;
        }

        if (glfwMods & GLFW_MOD_CONTROL)
        {
            result = result | NX::InputModifier::Control;
        }

        if (glfwMods & GLFW_MOD_ALT)
        {
            result = result | NX::InputModifier::Alt;
        }

        if (glfwMods & GLFW_MOD_SUPER)
        {
            result = result | NX::InputModifier::Super;
        }

        if (glfwMods & GLFW_MOD_CAPS_LOCK)
        {
            result = result | NX::InputModifier::CapsLock;
        }

        if (glfwMods & GLFW_MOD_NUM_LOCK)
        {
            result = result | NX::InputModifier::NumLock;
        }

        return result;
    }
} // namespace Platform
