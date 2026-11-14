// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "Player.h"

#include "NxWorld/Animations/FrameByFrame/FrameByFrameAnimator.h"
#include "Platform/Input/InputDevices/Keyboard.h"

using namespace Core;
using namespace NX;

ECS_IMPL(Player);

void Player::onTick(float delta)
{
    SceneObj::RectangleAnimated::onTick(delta);

    auto* _animator = findFirstChildOf<Animation::FrameByFrameAnimator>();

    using Key = Platform::Keyboard::Key;
    const auto isPressed = [](Key primary, Key secondary)
    {
        return Platform::Keyboard::IsKeyPressed(primary)
               || Platform::Keyboard::IsKeyPressed(secondary);
    };

    const int horizontal = static_cast<int>(isPressed(Key::D, Key::Right))
                           - static_cast<int>(isPressed(Key::A, Key::Left));
    const int vertical = static_cast<int>(isPressed(Key::W, Key::Up))
                         - static_cast<int>(isPressed(Key::S, Key::Down));

    constexpr float diagonalFactor = 0.70710678f;
    const float movementFactor = horizontal != 0 && vertical != 0 ? diagonalFactor : 1.f;

    const float rightMoveValue
        = static_cast<float>(horizontal) * _movementSpeed * movementFactor * delta;

    if (rightMoveValue != 0.f)
    {
        moveRight(rightMoveValue);
    }
    moveUp(static_cast<float>(vertical) * _movementSpeed * movementFactor * delta);

    StringAtom animation;
    if (vertical > 0)
    {
        animation = horizontal < 0   ? "Move UpLeft"_atom
                    : horizontal > 0 ? "Move UpRight"_atom
                                     : "Move Up"_atom;
    }
    else if (vertical < 0)
    {
        animation = horizontal < 0   ? "Move DownLeft"_atom
                    : horizontal > 0 ? "Move DownRight"_atom
                                     : "Move down"_atom;
    }
    else if (horizontal < 0)
    {
        animation = "Move left"_atom;
    }
    else if (horizontal > 0)
    {
        animation = "Move right"_atom;
    }

    if (!_animator)
    {
        return;
    }

    if (animation.isEmpty())
    {
        if (auto* activeAnimation = _animator->getActiveAnimation())
        {
            activeAnimation->stop();
        }
        return;
    }

    auto* activeAnimation = _animator->getActiveAnimation();
    if (_animator->getActiveAnimationName() != animation || !activeAnimation
        || !activeAnimation->isPlaying())
    {
        _animator->startAnimation(animation);
    }
}
