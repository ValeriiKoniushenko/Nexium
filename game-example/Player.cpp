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

using namespace Core;

ECS_IMPL(Player);

void Player::onInitialize()
{
    SceneObj::RectangleAnimated::onInitialize();

    _input = findFirstChildOf<InputController>();
    _animator = findFirstChildOf<Animation::FrameByFrameAnimator>();
}

void Player::onTick(float delta)
{
    RectangleAnimated::onTick(delta);

    if (_isGrounded)
    {
        if (!_animator->getAnimation("Santa_run")->isPlaying())
        {
            _animator->startAnimation("Santa_run");
        }
    }
    else
    {
        // TODO: Add animation "santa_jump"
    }

    if (_isGrounded && _input->isActionPressed("Jump"_atom))
    {
        _isGrounded = false;
        _velocityY = _jumpForce;
    }

    _velocityY += _gravity * delta;

    auto pos = getPosition();
    pos.y += _velocityY * delta;

    if (pos.y <= _groundCoords)
    {
        pos.y = _groundCoords;
        _velocityY = 0.f;
        _isGrounded = true;
    }

    setPosition(pos);
}