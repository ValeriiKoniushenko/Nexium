// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "Joystick.h"

#include "GLFW/glfw3.h"
#include "Platform/Input/Window.h"

#include <algorithm>

namespace Platform
{
    namespace
    {
        // Static state storage
        std::array<bool, Joystick::MaxJoysticks> s_connected{};
        std::array<std::array<bool, Joystick::MaxButtons>, Joystick::MaxJoysticks>
            s_currentButtons{};
        std::array<std::array<bool, Joystick::MaxButtons>, Joystick::MaxJoysticks>
            s_previousButtons{};
        std::array<std::array<float, Joystick::MaxAxes>, Joystick::MaxJoysticks> s_currentAxes{};
        std::array<std::array<float, Joystick::MaxAxes>, Joystick::MaxJoysticks> s_previousAxes{};
        float s_deadzone = Joystick::DefaultDeadzone;
    } // namespace

    void Joystick::poll()
    {
        for (int jid = 0; jid <= GLFW_JOYSTICK_LAST; ++jid)
        {
            if (!glfwJoystickPresent(jid))
            {
                if (s_connected[jid])
                {
                    s_connected[jid] = false;
                    s_currentButtons[jid].fill(false);
                    s_previousButtons[jid].fill(false);
                    s_currentAxes[jid].fill(0.f);
                    s_previousAxes[jid].fill(0.f);

                    const auto deviceId = static_cast<NX::InputDeviceId>(jid + 1);
                    NX::GamepadConnectionEvent event(deviceId);
                    event.setConnected(false);
                    GetWindow().onJoystickConnected->trigger(event);
                }
                continue;
            }

            if (glfwJoystickIsGamepad(jid) != GLFW_TRUE)
            {
                continue;
            }

            if (!s_connected[jid])
            {
                s_connected[jid] = true;
                s_currentButtons[jid].fill(false);
                s_previousButtons[jid].fill(false);
                s_currentAxes[jid].fill(0.f);
                s_previousAxes[jid].fill(0.f);

                const auto deviceId = static_cast<NX::InputDeviceId>(jid + 1);
                NX::GamepadConnectionEvent event(deviceId);
                event.setConnected(true);
                GetWindow().onJoystickConnected->trigger(event);
            }

            GLFWgamepadstate state{};
            if (!glfwGetGamepadState(jid, &state))
            {
                continue;
            }

            const auto deviceId = static_cast<NX::InputDeviceId>(jid + 1);

            // Buttons
            for (int btn = 0; btn <= GLFW_GAMEPAD_BUTTON_LAST; ++btn)
            {
                bool pressed = state.buttons[btn] == GLFW_PRESS;
                s_currentButtons[jid][btn] = pressed;

                bool wasPressed = s_previousButtons[jid][btn];
                if (pressed != wasPressed)
                {
                    NX::GamepadButtonEvent event(deviceId);
                    event.setButton(NX::InputButton(static_cast<uint16_t>(btn)));
                    event.setState(pressed ? NX::InputState::Pressed : NX::InputState::Released);
                    GetWindow().onJoystickButtonPressed->trigger(event);
                }
            }

            // Axes
            for (int ax = 0; ax <= GLFW_GAMEPAD_AXIS_LAST; ++ax)
            {
                float value = state.axes[ax];
                s_currentAxes[jid][ax] = value;

                float prev = s_previousAxes[jid][ax];
                if (std::abs(value - prev) > s_deadzone)
                {
                    NX::GamepadAxisEvent event(deviceId);
                    event.setAxis(static_cast<uint16_t>(ax));
                    event.setValue(value);
                    GetWindow().onJoystickAxisChanged->trigger(event);
                }
            }

            // Swap buffers for next frame
            s_previousButtons[jid] = s_currentButtons[jid];
            s_previousAxes[jid] = s_currentAxes[jid];
        }
    }

    bool Joystick::isConnected(int jid)
    {
        return jid >= 0 && jid <= GLFW_JOYSTICK_LAST && s_connected[jid];
    }

    bool Joystick::isButtonPressed(int jid, int button)
    {
        if (!isConnected(jid) || button < 0 || button >= MaxButtons)
        {
            return false;
        }
        return s_currentButtons[jid][button];
    }

    float Joystick::getAxis(int jid, int axis)
    {
        if (!isConnected(jid) || axis < 0 || axis >= MaxAxes)
        {
            return 0.f;
        }
        float value = s_currentAxes[jid][axis];
        return (std::abs(value) < s_deadzone) ? 0.f : value;
    }

    void Joystick::setDeadzone(float deadzone)
    {
        s_deadzone = deadzone;
    }

    float Joystick::getDeadzone()
    {
        return s_deadzone;
    }
} // namespace Platform