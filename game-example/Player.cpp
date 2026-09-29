/*
 * MIT License
 *
 * Copyright (c) 2018-2027 Valerii Koniushenko
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

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
