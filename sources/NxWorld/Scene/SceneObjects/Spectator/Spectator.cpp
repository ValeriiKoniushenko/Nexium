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

namespace NX
{

    ECS_IMPL(BaseSpectator);
    ECS_IMPL(Spectator2D);
    ECS_IMPL(Spectator3D);

    Core::StringAtom BaseSpectator::getCacheHash() const
    {
        return "EditorsRootSpectator";
    }

    void BaseSpectator::onTick(float delta)
    {
        Actor::onTick(delta);

        keyboardInput.update();
        mouseInput.update();
    }

    void BaseSpectator::onInitialize()
    {
        Actor::onInitialize();
    }

    void Spectator3D::onInitialize()
    {
        BaseSpectator::onInitialize();
        _inputController = InputController::Create("Spectator input"_atom, InputContext::Gameplay);
        const auto bindMovement = [this](const Core::StringAtom& name, Platform::Keyboard::Key key)
        { _inputController->bind(name, KeyChord::Exact(key), InputActionTrigger::WhileHeld); };
        bindMovement("Move forward"_atom, Platform::Keyboard::Key::W);
        bindMovement("Move backward"_atom, Platform::Keyboard::Key::S);
        bindMovement("Move right"_atom, Platform::Keyboard::Key::D);
        bindMovement("Move left"_atom, Platform::Keyboard::Key::A);
        bindMovement("Move up"_atom, Platform::Keyboard::Key::R);
        bindMovement("Move down"_atom, Platform::Keyboard::Key::F);
        _subscriptionPool << Platform::GetWindow().onMouseWheel->subscribeAndGetID(
            [this](glm::vec2 offset)
            {
                if (gGameInstance->isApplicationViewportFocused())
                {
                    auto mlt
                        = speed
                          / (Platform::Keyboard::IsKeyPressed(Platform::Keyboard::Key::Left_Shift)
                                 ? 8.f
                                 : 1.f);
                    moveForward(-offset.y * mlt * gGameInstance->world.getTimeDelta());
                }
            });
        _subscriptionPool << mouseInput.getOrCreate("mouseRotation", Platform::Mouse::Key::Right)
                                 ->onDrag->subscribeAndGetID(
                                     [this](glm::vec2 delta, auto)
                                     { yawAndPitch(delta * mouseSensitivity); });
    }

    void Spectator3D::onTick(float delta)
    {
        BaseSpectator::onTick(delta);
        if (!_inputController)
        {
            return;
        }
        // const float step
        //     = speed * delta
        //       / (Platform::Keyboard::IsKeyPressed(Platform::Keyboard::Key::Left_Shift)
        //                  ||
        //                  Platform::Keyboard::IsKeyPressed(Platform::Keyboard::Key::Right_Shift)
        //              ? 8.f
        //              : 1.f);
        // const auto pressed = [this](const Core::StringAtom& action)
        // { return static_cast<float>(_inputController->isActionPressed(action)); };
        // moveForward(step * (pressed("Move backward"_atom) - pressed("Move forward"_atom)));
        // moveRight(step * (pressed("Move right"_atom) - pressed("Move left"_atom)));
        // moveUp(step * (pressed("Move up"_atom) - pressed("Move down"_atom)));
    }

    void Spectator2D::onInitialize()
    {
        BaseSpectator::onInitialize();

        auto mouseMove = [this](glm::vec2 delta, MouseIA::SpecKeysState state)
        {
            auto mlt
                = (speed * (state.leftShift == Platform::Keyboard::KeyState::Pressed ? 5.f : 1.f))
                  * gGameInstance->world.getTimeDelta();

            moveRight(-delta.x * mlt);
            moveUp(delta.y * mlt);
        };
        _subscriptionPool << mouseInput
                                 .getOrCreate("spectatorMovement", Platform::Mouse::Key::Middle)
                                 ->onDrag->subscribeAndGetID(mouseMove);

        _subscriptionPool << mouseInput.onWheel->subscribeAndGetID(
            [this](glm::vec2 offset)
            {
                if (gGameInstance->isApplicationViewportFocused())
                {
                    float mlt = baseMlt;
                    if (Platform::Keyboard::IsKeyPressed(Platform::Keyboard::Key::Left_Shift))
                    {
                        mlt = leftShiftMlt;
                    }
                    offset *= mlt;

                    if (Platform::Keyboard::IsKeyPressed(Platform::Keyboard::Key::Left_Control))
                    {
                        if (auto* camera = findFirstChildOf<OrthographicCamera>())
                        {
                            const auto zoomStep = (maxZoom - minZoom) / 100.f;
                            const auto scrollStep = std::clamp(offset.y, -1.f, 1.f);
                            auto z = camera->getZoom();

                            auto finalStep = zoomStep * scrollStep * std::max(1.f, sqrtf(z));
                            const auto expectedZoom = z + finalStep;

                            if (expectedZoom < minZoom)
                            {
                                camera->setZoom(minZoom);
                                return;
                            }
                            if (expectedZoom > maxZoom)
                            {
                                camera->setZoom(maxZoom);
                                return;
                            }

                            camera->adjustZoom(finalStep);
                            return;
                        }
                    }

                    // 20 it's just a fake value to make touchpad moving closer to the Mouse
                    // feelings.
                    const float adjustedSpeed = speed * 10.f;
                    const float panSpeed = adjustedSpeed * gGameInstance->world.getTimeDelta();

                    moveRight(-offset.x * panSpeed);
                    moveUp(offset.y * panSpeed);
                }
            });
    }
} // namespace NX
