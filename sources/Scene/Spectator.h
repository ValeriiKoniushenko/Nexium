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

#pragma once

#include "../GameplaySystem/Entities/Actor.h"
#include "InputDevices/InputManager.h"

namespace Core
{
    class Spectator : public Actor
    {
        ECS_COMPONENT_DECL(Spectator, Actor);

    public:
        // TODO: Move it to Controller
        float speed = 600.f;
        float mouseSensitivity = 0.5f;

        KeyboardInputManger keyboardInput;
        MouseInputManger mouseInput;

    public:
        [[nodiscard]] StringAtom getCacheHash() const override;

    protected:
        DelegateSubscriberPoolGuard _subscriptionPool;

        void onTick(float delta) override;

        void onInitialize() override;
    };
} // namespace Core