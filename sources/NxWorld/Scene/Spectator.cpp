// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "Spectator.h"

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
        const auto getSpeed = [this](Platform::Keyboard::KeyState leftShift)
        { return speed / (leftShift == Platform::Keyboard::KeyState::Pressed ? 8.f : 1.f); };
        const auto bindMovement = [this, &getSpeed](const char* name, Platform::Keyboard::Key key,
                                                    auto movement, float direction)
        {
            _subscriptionPool << keyboardInput.getOrCreate(name, key)->onPress->subscribeAndGetID(
                [=](KeyboardIA::SpecKeysState state)
                {
                    movement(direction * getSpeed(state.leftShift)
                             * gGameInstance->world.getTimeDelta());
                });
        };

        bindMovement(
            "Move forward", Platform::Keyboard::Key::W, [this](float v) { moveForward(v); }, -1.f);
        bindMovement(
            "Move backward", Platform::Keyboard::Key::S, [this](float v) { moveForward(v); }, 1.f);
        bindMovement(
            "Move right", Platform::Keyboard::Key::D, [this](float v) { moveRight(v); }, 1.f);
        bindMovement(
            "Move left", Platform::Keyboard::Key::A, [this](float v) { moveRight(v); }, -1.f);
        bindMovement("Move up", Platform::Keyboard::Key::R, [this](float v) { moveUp(v); }, 1.f);
        bindMovement("Move down", Platform::Keyboard::Key::F, [this](float v) { moveUp(v); }, -1.f);
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
                            const auto finalStep = zoomStep * scrollStep;

                            if (camera->getZoom() + finalStep < minZoom
                                || camera->getZoom() + finalStep > maxZoom)
                            {
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
