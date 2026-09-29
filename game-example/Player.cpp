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
#include "NxWorld/Framework/InputController.h"

using namespace Core;
using namespace NX;

ECS_IMPL(Player);

void Player::onTick(float delta)
{
    SceneObj::RectangleAnimated::onTick(delta);

    const auto* _input = findFirstChildOf<InputController>();
    auto* _animator = findFirstChildOf<Animation::FrameByFrameAnimator>();

    if (!_input || !_input->isEnabled())
    {
        return;
    }

    const int horizontal = static_cast<int>(_input->isActionPressed("Move right"_atom))
                           - static_cast<int>(_input->isActionPressed("Move left"_atom));
    const int vertical = static_cast<int>(_input->isActionPressed("Move up"_atom))
                         - static_cast<int>(_input->isActionPressed("Move down"_atom));

    constexpr float diagonalFactor = 0.70710678f;
    const float movementFactor = horizontal != 0 && vertical != 0 ? diagonalFactor : 1.f;
    moveRight(static_cast<float>(horizontal) * _movementSpeed * movementFactor * delta);
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
