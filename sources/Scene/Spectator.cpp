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

#include "Spectator.h"

#include "Editor/Gizmo.h"
#include "GameplaySystem/Framework/GameInstance.h"

namespace Core
{
    R_FRIEND_IMPL(Spectator);
    ECS_COMPONENT_IMPL(Spectator);

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

        (void)addUniqueTypeChildComponent<BaseCamera>();

        const auto getSpeed = [this](KeyboardIA::SpecKeysState state)
        { return speed / (state.leftShift == Keyboard::KeyState::Pressed ? 8.f : 1.f); };

        const auto bindMovement
            = [this, &getSpeed](const char* name, Keyboard::Key key, auto movement, float direction)
        {
            _subscriptionPool << keyboardInput.getOrCreate(name, key)->onPress->subscribeAndGetID(
                [=](KeyboardIA::SpecKeysState state)
                { movement(direction * getSpeed(state) * gGameInstance->world.timeDelta); });
        };

        bindMovement("Move forward", Keyboard::Key::W, [this](float v) { moveForward(v); }, -1.f);
        bindMovement("Move backward", Keyboard::Key::S, [this](float v) { moveForward(v); }, 1.f);
        bindMovement("Move right", Keyboard::Key::D, [this](float v) { moveRight(v); }, 1.f);
        bindMovement("Move left", Keyboard::Key::A, [this](float v) { moveRight(v); }, -1.f);
        bindMovement("Move up", Keyboard::Key::R, [this](float v) { moveUp(v); }, 1.f);
        bindMovement("Move down", Keyboard::Key::F, [this](float v) { moveUp(v); }, -1.f);

        _subscriptionPool << mouseInput.getOrCreate("mouseRotation", Mouse::Key::Right)
                                 ->onDrag->subscribeAndGetID(
                                     [this](glm::vec2 delta, auto)
                                     {
                                         if (!gDragDrop.isTypeOf<Gizmo::DragData>())
                                         {
                                             yawAndPitch(delta * mouseSensitivity);
                                         }
                                     });
    }
} // namespace Core