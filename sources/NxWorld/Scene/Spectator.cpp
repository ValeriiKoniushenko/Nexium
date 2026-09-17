// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "Spectator.h"

#include "NxWorld/Framework/GameInstance.h"

namespace NX
{

    ECS_IMPL(Spectator);

    Core::StringAtom Spectator::getCacheHash() const
    {
        return "EditorsRootSpectator";
    }

    void Spectator::onTick(float delta)
    {
        Actor::onTick(delta);

        keyboardInput.update();
        mouseInput.update();
    }

    void Spectator::onInitialize()
    {
        Actor::onInitialize();

        const auto getSpeed = [this](KeyboardIA::SpecKeysState state)
        { return speed / (state.leftShift == Platform::Keyboard::KeyState::Pressed ? 8.f : 1.f); };

        const auto bindMovement = [this, &getSpeed](const char* name, Platform::Keyboard::Key key,
                                                    auto movement, float direction)
        {
            _subscriptionPool << keyboardInput.getOrCreate(name, key)->onPress->subscribeAndGetID(
                [=](KeyboardIA::SpecKeysState state)
                { movement(direction * getSpeed(state) * gGameInstance->world.getTimeDelta()); });
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

        // TODO: awful approach with direct window call. Refactor.
        _subscriptionPool << Platform::GetWindow().onMouseWheel->subscribeAndGetID(
            [s = Core::WeakPtr(this)](glm::vec2 offset)
            {
                if (s)
                {
                    if (gGameInstance->isApplicationViewportFocused())
                    {
                        if (auto obj = s.tryLoad())
                        {
                            auto mlt = obj->speed
                                       / (Platform::Keyboard::IsKeyPressed(
                                              Platform::Keyboard::Key::Left_Shift)
                                              ? 8.f
                                              : 1.f);
                            obj->moveForward(-offset.y * mlt * gGameInstance->world.getTimeDelta());
                        }
                    }
                }
            });

        _subscriptionPool << mouseInput.getOrCreate("mouseRotation", Platform::Mouse::Key::Right)
                                 ->onDrag->subscribeAndGetID(
                                     [this](glm::vec2 delta, auto)
                                     { yawAndPitch(delta * mouseSensitivity); });
    }
} // namespace NX
