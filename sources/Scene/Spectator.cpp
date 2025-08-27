// MIT License
//
// Copyright (c) 2019-2025 Valerii Koniushenko
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "Spectator.h"

#include "Editor/Windows/GameViewport.h"
#include "GameplaySystem/Framework/GameInstance.h"

namespace Core
{
    ECS_REGISTER_NEW_COMPONENT_TYPE(Spectator)

    nlohmann::json Spectator::toJson() const
    {
        auto json = Actor::toJson();
        json["speed"] = speed;
        json["mouseSensitivity"] = mouseSensitivity;
        json["keyboardInput"] = keyboardInput.toJson();
        // json["mouseInput"] = mouseInput.toJson();

        return json;
    }

    void Spectator::fromJson(const nlohmann::json& json, bool isIgnoreChildren)
    {
        Actor::fromJson(json, isIgnoreChildren);

        if (json.contains("speed"))
        {
            speed = json["speed"];
        }
        if (json.contains("mouseSensitivity"))
        {
            mouseSensitivity = json["mouseSensitivity"];
        }
        if (json.contains("keyboardInput"))
        {
            keyboardInput.fromJson(json["keyboardInput"], isIgnoreChildren);
        }
    }

    StringAtom Spectator::getCacheHash() const
    {
        return "EditorsRootSpectator";
    }

    nlohmann::json Spectator::toCacheData() const
    {
        return toJson();
    }

    void Spectator::fromCacheData(const nlohmann::json& json)
    {
        fromJson(json, false);
    }

    void Spectator::onTick(float delta)
    {
        Actor::onTick(delta);

        keyboardInput.update();
        mouseInput.update();
    }

    void Spectator::onInit()
    {
        Actor::onInit();

        tryReadFromCache();

        BaseCamera& camera = *getOrAddChildComponent<BaseCamera>();

        static auto getRealSpeed = [this](KeyboardIA::SpecKeysState state)
        {
            const float mlt = state.leftShift.cast() == Keyboard::KeyState::Pressed ? 10.f : 1.f;
            return speed * mlt;
        };

        auto toggleRenderMode
            = keyboardInput.getOrCreate("toggleRenderMode", Keyboard::Key::Key_F1);
        toggleRenderMode->setIsRepeatable(false);
        toggleRenderMode->onPress.subscribe(
            [](auto)
            {
                gGameInstance->toggleRenderMode();
            });

        // clang-format off
        keyboardInput.getOrCreate("moveForward", Keyboard::Key::Key_W)->onPress.subscribe([&](auto state){ camera.moveForward(-getRealSpeed(state) * gGameInstance->world.timeDelta); });
        keyboardInput.getOrCreate("moveBackward", Keyboard::Key::Key_S)->onPress.subscribe([&](auto state){ camera.moveForward(getRealSpeed(state) * gGameInstance->world.timeDelta); });
        keyboardInput.getOrCreate("moveRight", Keyboard::Key::Key_D)->onPress.subscribe([&](auto state){ camera.moveRight(getRealSpeed(state) * gGameInstance->world.timeDelta); });
        keyboardInput.getOrCreate("moveLeft", Keyboard::Key::Key_A)->onPress.subscribe([&](auto state){ camera.moveRight(-getRealSpeed(state) * gGameInstance->world.timeDelta); });
        keyboardInput.getOrCreate("moveUp", Keyboard::Key::Key_Space)->onPress.subscribe([&](auto state){ camera.moveUp(getRealSpeed(state) * gGameInstance->world.timeDelta); });
        keyboardInput.getOrCreate("moveDown", Keyboard::Key::Key_C)->onPress.subscribe([&](auto state){ camera.moveUp(-getRealSpeed(state) * gGameInstance->world.timeDelta); });
        // clang-format on

        mouseInput.getOrCreate("mouseRotation", Core::Mouse::Key_None)
            ->onDrag.subscribe(
                [this](glm::vec2 delta, auto spec)
                {
                    if (gGameInstance->currentCamera)
                    {
                        gGameInstance->currentCamera->yawAndPitch(delta * mouseSensitivity);
                    }
                });
    }
} // namespace Core