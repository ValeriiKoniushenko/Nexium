// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "Spectator.h"

#include "Core/Math.h"
#include "NxWorld/Entities/Camera/Camera.h"
#include "NxWorld/Framework/GameInstance.h"
#include "Platform/Input/InputDevices/Joystick.h"
#include "Platform/Input/InputDevices/Keyboard.h"
#include "Platform/Input/InputDevices/Mouse.h"

namespace
{
    glm::vec2 RadialDeadzone(glm::vec2 value, float deadzone)
    {
        return glm::length(value) < deadzone ? glm::vec2{ 0.f } : value;
    }
} // namespace

namespace NX
{

    ECS_IMPL(BaseSpectator);
    ECS_IMPL(Spectator2D);
    ECS_IMPL(Spectator3D);

    Core::StringAtom BaseSpectator::getCacheHash() const
    {
        return "EditorsRootSpectator";
    }

    void BaseSpectator::onInitialize()
    {
        Actor::onInitialize();

        _input = findFirstChildOf<InputController>();
        if (!_input)
        {
            _input = addChildComponent<InputController>("Input"_atom);
        }
        if (_input)
        {
            setupInputActions(*_input);
        }
        updateInputControllerState();
    }

    void BaseSpectator::onSceneActivated()
    {
        _isSceneActive = true;
        updateInputControllerState();
    }

    void BaseSpectator::onSceneDeactivated()
    {
        _isSceneActive = false;
        updateInputControllerState();
    }

    void BaseSpectator::updateInputControllerState()
    {
        if (_input)
        {
            _input->setEnabled(_isSceneActive);
        }
    }

    void Spectator2D::setupInputActions(InputController& input)
    {
        using Axis = Platform::Joystick::GamepadAxis;
        using Key = Platform::Keyboard::Key;

        input.addAction("Pan"_atom, { InputBinding::MouseButton(Platform::Mouse::Key::Middle) });
        input.addAction("PointerDelta"_atom, { InputBinding::MouseDelta() });
        input.addAction("Scroll"_atom, { InputBinding::MouseScroll() });
        input.addAction("Boost"_atom, { InputBinding::Key(Key::Left_Shift),
                                        InputBinding::GamepadButton(
                                            Platform::Joystick::GamepadButton::LeftBumper) });
        input.addAction("Zoom"_atom, { InputBinding::Key(Key::Left_Control) });
        input.addAction("Move"_atom,
                        { InputBinding::GamepadAxis(Axis::LeftX),
                          InputBinding::GamepadAxis(Axis::LeftY).negated().swizzled() });
        input.addAction("GamepadZoom"_atom,
                        { InputBinding::GamepadAxis(Axis::RightTrigger),
                          InputBinding::GamepadAxis(Axis::LeftTrigger).negated() });
    }

    void Spectator2D::onTick(float delta)
    {
        BaseSpectator::onTick(delta);

        auto* input = getInputController();
        if (!input)
        {
            return;
        }

        const bool boost = input->isActive("Boost"_atom);
        if (input->isActive("Pan"_atom))
        {
            const auto drag = input->getValue<glm::vec2>("PointerDelta"_atom);
            const auto mlt = (speed * (boost ? 5.f : 1.f)) * gGameInstance->world.getTimeDelta();

            moveRight(-drag.x * mlt);
            moveUp(drag.y * mlt);
        }

        if (!gGameInstance->isApplicationViewportFocused())
        {
            return;
        }

        const auto scroll = input->getValue<glm::vec2>("Scroll"_atom);
        if (scroll != glm::vec2{ 0.f })
        {
            applyScroll(scroll, boost, input->isActive("Zoom"_atom));
        }

        applyGamepad(*input, boost);
    }

    void Spectator2D::applyGamepad(const InputController& input, bool boost)
    {
        const auto move = RadialDeadzone(input.getValue<glm::vec2>("Move"_atom), gamepadDeadzone);
        if (move != glm::vec2{ 0.f })
        {
            const auto step = speed * (boost ? 5.f : 1.f) * gGameInstance->world.getTimeDelta();
            moveRight(move.x * step);
            moveUp(move.y * step);
        }

        const auto zoom = input.getValue<float>("GamepadZoom"_atom);
        if (std::abs(zoom) >= gamepadDeadzone)
        {
            applyZoom(zoom * gamepadZoomSpeed * gGameInstance->world.getTimeDelta());
        }
    }

    bool Spectator2D::applyZoom(float scrollStep)
    {
        auto* camera = findFirstChildOf<OrthographicCamera>();
        if (!camera)
        {
            return false;
        }

        const auto zoomStep = (maxZoom - minZoom) / 100.f;
        const auto z = camera->getZoom();
        const auto finalStep = zoomStep * scrollStep * std::max(1.f, sqrtf(z));
        const auto expectedZoom = z + finalStep;

        if (expectedZoom < minZoom)
        {
            camera->setZoom(minZoom);
        }
        else if (expectedZoom > maxZoom)
        {
            camera->setZoom(maxZoom);
        }
        else
        {
            camera->adjustZoom(finalStep);
        }
        return true;
    }

    void Spectator2D::applyScroll(glm::vec2 offset, bool boost, bool zoom)
    {
        offset *= boost ? leftShiftMlt : baseMlt;

        if (zoom && applyZoom(std::clamp(offset.y, -1.f, 1.f)))
        {
            return;
        }

        // 20 it's just a fake value to make touchpad moving closer to the Mouse
        // feelings.
        const float adjustedSpeed = speed * 10.f;
        const float panSpeed = adjustedSpeed * gGameInstance->world.getTimeDelta();

        moveRight(-offset.x * panSpeed);
        moveUp(offset.y * panSpeed);
    }

    void Spectator3D::setupInputActions(InputController& input)
    {
        using Axis = Platform::Joystick::GamepadAxis;

        input.addAction("Rotate"_atom, { InputBinding::MouseButton(Platform::Mouse::Key::Right) });
        input.addAction("PointerDelta"_atom, { InputBinding::MouseDelta() });
        input.addAction("Move"_atom, { InputBinding::GamepadAxis(Axis::LeftX),
                                       InputBinding::GamepadAxis(Axis::LeftY).swizzled() });
        input.addAction("Look"_atom, { InputBinding::GamepadAxis(Axis::RightX),
                                       InputBinding::GamepadAxis(Axis::RightY).swizzled() });
    }

    void Spectator3D::onTick(float delta)
    {
        BaseSpectator::onTick(delta);

        auto* input = getInputController();
        if (!input)
        {
            return;
        }

        if (input->isActive("Rotate"_atom))
        {
            yawAndPitch(input->getValue<glm::vec2>("PointerDelta"_atom) * mouseSensitivity);
        }

        if (!gGameInstance->isApplicationViewportFocused())
        {
            return;
        }

        const float timeDelta = gGameInstance->world.getTimeDelta();
        const auto move = RadialDeadzone(input->getValue<glm::vec2>("Move"_atom), gamepadDeadzone);
        if (move != glm::vec2{ 0.f })
        {
            moveForward(move.y * speed * timeDelta);
            moveRight(move.x * speed * timeDelta);
        }

        const auto look = RadialDeadzone(input->getValue<glm::vec2>("Look"_atom), gamepadDeadzone);
        if (look != glm::vec2{ 0.f })
        {
            yawAndPitch(look * gamepadLookSpeed * timeDelta);
        }
    }
} // namespace NX
