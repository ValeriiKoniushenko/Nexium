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

namespace Core
{

    ECS_IMPL(Spectator);

    StringAtom Spectator::getCacheHash() const
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
        { return speed / (state.leftShift == Keyboard::KeyState::Pressed ? 8.f : 1.f); };

        const auto bindMovement
            = [this, &getSpeed](const char* name, Keyboard::Key key, auto movement, float direction)
        {
            _subscriptionPool << keyboardInput.getOrCreate(name, key)->onPress->subscribeAndGetID(
                [=](KeyboardIA::SpecKeysState state)
                { movement(direction * getSpeed(state) * gGameInstance->world.getTimeDelta()); });
        };

        bindMovement("Move forward", Keyboard::Key::W, [this](float v) { moveForward(v); }, -1.f);
        bindMovement("Move backward", Keyboard::Key::S, [this](float v) { moveForward(v); }, 1.f);
        bindMovement("Move right", Keyboard::Key::D, [this](float v) { moveRight(v); }, 1.f);
        bindMovement("Move left", Keyboard::Key::A, [this](float v) { moveRight(v); }, -1.f);
        bindMovement("Move up", Keyboard::Key::R, [this](float v) { moveUp(v); }, 1.f);
        bindMovement("Move down", Keyboard::Key::F, [this](float v) { moveUp(v); }, -1.f);

        // TODO: awful approach with direct window call. Refactor.
        _subscriptionPool << GetWindow().onMouseWheel->subscribeAndGetID(
            [s = WeakPtr(this)](glm::vec2 offset)
            {
                if (s)
                {
                    if (gGameInstance->isApplicationViewportFocused())
                    {
                        if (auto obj = s.tryLoad())
                        {
                            auto mlt
                                = obj->speed
                                  / (Keyboard::IsKeyPressed(Keyboard::Key::Left_Shift) ? 8.f : 1.f);
                            obj->moveForward(-offset.y * mlt * gGameInstance->world.getTimeDelta());
                        }
                    }
                }
            });

        _subscriptionPool << mouseInput.getOrCreate("mouseRotation", Mouse::Key::Right)
                                 ->onDrag->subscribeAndGetID(
                                     [this](glm::vec2 delta, auto)
                                     { yawAndPitch(delta * mouseSensitivity); });
    }
} // namespace Core
