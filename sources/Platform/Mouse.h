// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "Glfw.h"
#include "JustReflectMe/Adapter.h"
#include "glm/glm.hpp"

#include <cstdint>

namespace Platform
{
    class Mouse
    {
    public:
        ENUM_CLASS();
        enum class Mod
        {
            None = 0,
            Shift = GLFW_MOD_SHIFT,
            Control = GLFW_MOD_CONTROL,
            Alt = GLFW_MOD_ALT,
            Super = GLFW_MOD_SUPER,
            CapsLock = GLFW_MOD_CAPS_LOCK,
            NumLock = GLFW_MOD_NUM_LOCK
        };

        ENUM_CLASS();
        enum class Key
        {
            _1 = GLFW_MOUSE_BUTTON_1,
            _2 = GLFW_MOUSE_BUTTON_2,
            _3 = GLFW_MOUSE_BUTTON_3,
            _4 = GLFW_MOUSE_BUTTON_4,
            _5 = GLFW_MOUSE_BUTTON_5,
            _6 = GLFW_MOUSE_BUTTON_6,
            _7 = GLFW_MOUSE_BUTTON_7,
            _8 = GLFW_MOUSE_BUTTON_8,
            Last = GLFW_MOUSE_BUTTON_LAST,
            Left = GLFW_MOUSE_BUTTON_LEFT,
            Right = GLFW_MOUSE_BUTTON_RIGHT,
            Middle = GLFW_MOUSE_BUTTON_MIDDLE,
            None = ~0
        };

        ENUM_CLASS();
        enum class State : std::int32_t
        {
            None = 0,
            Press = GLFW_PRESS,
            Release = GLFW_RELEASE,
        };

    public:
        Mouse() = delete;

        [[nodiscard]] static glm::vec2 GetPosition();

        [[nodiscard]] static glm::vec2 GetInViewportPosition();

        [[nodiscard]] static bool IsKeyPressed(Key key);

        [[nodiscard]] static bool IsKeyReleased(Key key);

        [[nodiscard]] static bool isKeyRepeated(Key key);
    };
} // namespace Platform

#include "Mouse.generated.h" // added by the code generator. Better don't move it.
