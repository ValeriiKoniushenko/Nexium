/*
 * MIT License
 *
 * Copyright (c) 2018-2025 Valerii Koniushenko
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

#include "GameplaySystem/Framework/GameInstance.h"

namespace Core
{
    ECS_COMPONENT_IMPL(Spectator)

    void Spectator::ioFieldsUpdate(DataStream& stream)
    {
        Actor::ioFieldsUpdate(stream);
        stream.field("speed", speed);
        stream.field("mouseSensitivity", mouseSensitivity);
        stream.field(keyboardInput);
        // stream.field("mouseInput", mouseInput);
    }

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

        tryReadFromCache();

        BaseCamera& camera = *getOrAddChildComponent<BaseCamera>();

        static auto getRealSpeed = [this](KeyboardIA::SpecKeysState state)
        {
            const float div = state.leftShift.cast() == Keyboard::KeyState::Pressed ? 8.f : 1.f;
            return speed / div;
        };

        // clang-format off
        keyboardInput.getOrCreate("Move forward", Keyboard::Key::Key_W)->onPress.subscribe([&](auto state){ camera.moveForward(-getRealSpeed(state) * gGameInstance->world.timeDelta); });
        keyboardInput.getOrCreate("Move backward", Keyboard::Key::Key_S)->onPress.subscribe([&](auto state){ camera.moveForward(getRealSpeed(state) * gGameInstance->world.timeDelta); });
        keyboardInput.getOrCreate("Move right", Keyboard::Key::Key_D)->onPress.subscribe([&](auto state){ camera.moveRight(getRealSpeed(state) * gGameInstance->world.timeDelta); });
        keyboardInput.getOrCreate("Move left", Keyboard::Key::Key_A)->onPress.subscribe([&](auto state){ camera.moveRight(-getRealSpeed(state) * gGameInstance->world.timeDelta); });
        keyboardInput.getOrCreate("Move up", Keyboard::Key::Key_R)->onPress.subscribe([&](auto state){ camera.moveUp(getRealSpeed(state) * gGameInstance->world.timeDelta); });
        keyboardInput.getOrCreate("Move down", Keyboard::Key::Key_F)->onPress.subscribe([&](auto state){ camera.moveUp(-getRealSpeed(state) * gGameInstance->world.timeDelta); });
        // clang-format on

        mouseInput.getOrCreate("mouseRotation", Mouse::Key_Right)
            ->onDrag.subscribe(
                [this](glm::vec2 delta, auto spec)
                {
                    if (gGameInstance->currentCamera && !gDragDrop.isTypeOf<Gizmo::DragData>())
                    {
                        gGameInstance->currentCamera->yawAndPitch(delta * mouseSensitivity);
                    }
                });
    }
} // namespace Core