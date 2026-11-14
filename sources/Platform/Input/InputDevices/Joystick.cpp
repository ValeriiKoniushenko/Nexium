// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "Joystick.h"

#include "Platform/Input/Window.h"

#include <algorithm>
#include <cmath>

namespace Platform
{
    namespace
    {
        // Connection state.
        std::array<bool, Joystick::MaxJoysticks> s_connected{};

        // Button states.
        std::array<std::array<bool, Joystick::MaxButtons>, Joystick::MaxJoysticks>
            s_currentButtons{};

        std::array<std::array<bool, Joystick::MaxButtons>, Joystick::MaxJoysticks>
            s_previousButtons{};

        // Axis states.
        std::array<std::array<float, Joystick::MaxAxes>, Joystick::MaxJoysticks> s_currentAxes{};

        // Last values for which axis change events were generated.
        std::array<std::array<float, Joystick::MaxAxes>, Joystick::MaxJoysticks>
            s_lastReportedAxes{};

        // Configuration.
        float s_axisChangeThreshold = Joystick::DefaultAxisChangeThreshold;

    } // namespace

    void Joystick::poll()
    {
        for (int jid = 0; jid <= GLFW_JOYSTICK_LAST; ++jid)
        {
            // Check physical connection.
            if (!glfwJoystickPresent(jid))
            {
                disconnect(jid);
                continue;
            }

            // Only standard GLFW gamepads are currently supported.
            if (glfwJoystickIsGamepad(jid) != GLFW_TRUE)
            {
                disconnect(jid);
                continue;
            }

            // Handle new connection.
            if (!s_connected[jid])
            {
                resetState(jid);
                s_connected[jid] = true;

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

            // -------------------------------------------------
            // Buttons
            // -------------------------------------------------

            for (int btn = 0; btn <= GLFW_GAMEPAD_BUTTON_LAST; ++btn)
            {
                const bool pressed = state.buttons[btn] == GLFW_PRESS;

                s_currentButtons[jid][btn] = pressed;

                const bool wasPressed = s_previousButtons[jid][btn];

                if (pressed == wasPressed)
                {
                    continue;
                }

                NX::GamepadButtonEvent event(deviceId);

                event.setButton(NX::InputButton(static_cast<uint16_t>(btn)));

                event.setState(pressed ? NX::InputState::Pressed : NX::InputState::Released);

                GetWindow().onJoystickButtonPressed->trigger(event);
            }

            // -------------------------------------------------
            // Axes
            // -------------------------------------------------

            for (int ax = 0; ax <= GLFW_GAMEPAD_AXIS_LAST; ++ax)
            {
                const float value = state.axes[ax];

                // Always keep the latest physical state.
                s_currentAxes[jid][ax] = value;

                const float lastReported = s_lastReportedAxes[jid][ax];

                // Compare against the last reported value, not the previous frame.
                const float difference = std::abs(value - lastReported);

                if (difference <= s_axisChangeThreshold)
                {
                    continue;
                }

                NX::GamepadAxisEvent event(deviceId);

                event.setAxis(static_cast<uint16_t>(ax));
                event.setValue(value);

                GetWindow().onJoystickAxisChanged->trigger(event);

                // Update the reference only after generating an event.
                s_lastReportedAxes[jid][ax] = value;
            }

            // Store button states for the next frame.
            s_previousButtons[jid] = s_currentButtons[jid];
        }
    }

    void Joystick::disconnect(int jid)
    {
        if (!isConnected(jid))
        {
            return;
        }

        const auto deviceId = static_cast<NX::InputDeviceId>(jid + 1);

        // Release all buttons that were pressed before disconnection.
        for (int btn = 0; btn < MaxButtons; ++btn)
        {
            if (!s_currentButtons[jid][btn])
            {
                continue;
            }

            NX::GamepadButtonEvent event(deviceId);

            event.setButton(NX::InputButton(static_cast<uint16_t>(btn)));
            event.setState(NX::InputState::Released);

            GetWindow().onJoystickButtonPressed->trigger(event);
        }

        // Return axes that are not at rest to their rest values.
        for (int ax = 0; ax < MaxAxes; ++ax)
        {
            const float rest = getAxisRestValue(ax);
            if (s_lastReportedAxes[jid][ax] == rest)
            {
                continue;
            }

            NX::GamepadAxisEvent event(deviceId);

            event.setAxis(static_cast<uint16_t>(ax));
            event.setValue(rest);

            GetWindow().onJoystickAxisChanged->trigger(event);
        }

        s_connected[jid] = false;

        resetState(jid);

        // Notify subscribers after releasing all active inputs.
        NX::GamepadConnectionEvent event(deviceId);
        event.setConnected(false);

        GetWindow().onJoystickConnected->trigger(event);
    }

    void Joystick::resetState(int jid)
    {
        if (jid < 0 || jid >= MaxJoysticks)
        {
            return;
        }

        s_currentButtons[jid].fill(false);
        s_previousButtons[jid].fill(false);

        for (int ax = 0; ax < MaxAxes; ++ax)
        {
            s_currentAxes[jid][ax] = getAxisRestValue(ax);
            s_lastReportedAxes[jid][ax] = getAxisRestValue(ax);
        }
    }

    bool Joystick::isConnected(int jid)
    {
        return jid >= 0 && jid < MaxJoysticks && s_connected[jid];
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
        if (axis < 0 || axis >= MaxAxes)
        {
            return 0.f;
        }

        if (!isConnected(jid))
        {
            return getAxisRestValue(axis);
        }

        return s_currentAxes[jid][axis];
    }

    void Joystick::setAxisChangeThreshold(float threshold)
    {
        s_axisChangeThreshold = std::clamp(threshold, 0.f, 2.f);
    }

    float Joystick::getAxisChangeThreshold()
    {
        return s_axisChangeThreshold;
    }

    float Joystick::getAxisRestValue(int axis) noexcept
    {
        const bool isTrigger
            = axis == GLFW_GAMEPAD_AXIS_LEFT_TRIGGER || axis == GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER;
        return isTrigger ? -1.f : 0.f;
    }

} // namespace Platform