// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "NxSubsystems/Input/InputManager.h"
#include "NxWorld/Entities/Actor.h"

namespace NX
{
    CLASS();
    class Spectator : public Actor
    {
        ECS_DECL(Spectator, NX::Actor);

    public:
        // TODO: Move it to Controller
        FIELD();
        float speed = 600.f;

        FIELD();
        float mouseSensitivity = 0.5f;

        KeyboardInputManger keyboardInput;
        MouseInputManger mouseInput;

    public:
        [[nodiscard]] Core::StringAtom getCacheHash() const override;

    protected:
        void onTick(float delta) override;
        Core::DelegateSubscriberPoolGuard _subscriptionPool;

        void onInitialize() override;
    };
} // namespace NX

#include "Spectator.generated.h" // added by the code generator. Better don't move it.
