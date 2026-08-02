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

#include "Animations/FrameByFrame/FrameByFrameAnimator.h"
#include "GameplaySystem/Framework/GameInstance.h"
#include "InputDevices/InputAction.h"

namespace GameExample
{
    ECS_IMPL(Player);

    void Player::onInitialize()
    {
        Rectangle::onInitialize();

        auto* animator = findFirstChildOf<Core::Animation::FrameByFrameAnimator>();

        static constexpr int speed = 100;

        const auto getSpeed = [this](Core::KeyboardIA::SpecKeysState state)
        { return speed / (state.leftShift == Core::Keyboard::KeyState::Pressed ? 8.f : 1.f); };
        const auto bindMovement = [this, &getSpeed](const char* name, Core::Keyboard::Key key,
                                                    auto movement, float direction)
        {
            _subscriptionPool << keyboardInput.getOrCreate(name, key)->onPress->subscribeAndGetID(
                [=](Core::KeyboardIA::SpecKeysState state)
                { movement(direction * getSpeed(state) * gGameInstance->world.getTimeDelta()); });
        };

        bindMovement(
            "Move right", Core::Keyboard::Key::Right,
            [this, animator](float v)
            {
                if (animator && animator->getActiveAnimationName() != "Walk_right"_atom)
                {
                    animator->startAnimation("Walk_right"_atom);
                }
                moveRight(v);
            },
            1.f);
        bindMovement(
            "Move left", Core::Keyboard::Key::Left,
            [this, animator](float v)
            {
                if (animator && animator->getActiveAnimationName() != "Walk_left"_atom)
                {
                    animator->startAnimation("Walk_left"_atom);
                }
                moveRight(v);
            },
            -1.f);
    }
    void Player::onTick(float delta)
    {
        Rectangle::onTick(delta);
        keyboardInput.update();
    }

} // namespace GameExample
