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

#include "JustReflectMe/Adapter.h"
#include "Platform/Input/Events/GamepadEvents.h"
#include "Platform/Input/Events/InputEvent.h"
#include "Platform/Input/Glfw.h"

#include <array>
#include <cstdint>

namespace Platform
{
    class Joystick
    {
    public:
        static constexpr int MaxJoysticks = GLFW_JOYSTICK_LAST + 1;
        static constexpr int MaxButtons = GLFW_GAMEPAD_BUTTON_LAST + 1;
        static constexpr int MaxAxes = GLFW_GAMEPAD_AXIS_LAST + 1;

        static constexpr float DefaultDeadzone = 0.15f;
        static constexpr float DefaultAxisChangeThreshold = 0.001f;

        Joystick() = delete;

        /// Call once per frame after glfwPollEvents() to update state and generate events.
        static void poll();

        /// Check if a gamepad is connected at the given joystick index.
        [[nodiscard]] static bool isConnected(int jid);

        /// Get current button state.
        [[nodiscard]] static bool isButtonPressed(int jid, int button);

        /// Get current axis value with deadzone applied.
        [[nodiscard]] static float getAxis(int jid, int axis);

        /// Set deadzone applied to axis values returned by getAxis().
        static void setDeadzone(float deadzone);

        [[nodiscard]] static float getDeadzone();

        /// Set minimum axis value difference required to generate an event.
        static void setAxisChangeThreshold(float threshold);

        [[nodiscard]] static float getAxisChangeThreshold();

        // GLFW gamepad button/axis constants for convenience.
        enum class GamepadButton : uint16_t
        {
            A = GLFW_GAMEPAD_BUTTON_A,
            B = GLFW_GAMEPAD_BUTTON_B,
            X = GLFW_GAMEPAD_BUTTON_X,
            Y = GLFW_GAMEPAD_BUTTON_Y,
            LeftBumper = GLFW_GAMEPAD_BUTTON_LEFT_BUMPER,
            RightBumper = GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER,
            Back = GLFW_GAMEPAD_BUTTON_BACK,
            Start = GLFW_GAMEPAD_BUTTON_START,
            Guide = GLFW_GAMEPAD_BUTTON_GUIDE,
            LeftThumb = GLFW_GAMEPAD_BUTTON_LEFT_THUMB,
            RightThumb = GLFW_GAMEPAD_BUTTON_RIGHT_THUMB,
            DpadUp = GLFW_GAMEPAD_BUTTON_DPAD_UP,
            DpadRight = GLFW_GAMEPAD_BUTTON_DPAD_RIGHT,
            DpadDown = GLFW_GAMEPAD_BUTTON_DPAD_DOWN,
            DpadLeft = GLFW_GAMEPAD_BUTTON_DPAD_LEFT,
            Last = GLFW_GAMEPAD_BUTTON_LAST
        };

        enum class GamepadAxis : uint16_t
        {
            LeftX = GLFW_GAMEPAD_AXIS_LEFT_X,
            LeftY = GLFW_GAMEPAD_AXIS_LEFT_Y,
            RightX = GLFW_GAMEPAD_AXIS_RIGHT_X,
            RightY = GLFW_GAMEPAD_AXIS_RIGHT_Y,
            LeftTrigger = GLFW_GAMEPAD_AXIS_LEFT_TRIGGER,
            RightTrigger = GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER,
            Last = GLFW_GAMEPAD_AXIS_LAST
        };

    private:
        static void resetState(int jid);
        static void disconnect(int jid);
    };
} // namespace Platform