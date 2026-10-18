// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

// Module: Nexium::World

#pragma once

#include "NxWorld/Entities/Actor.h"
#include "NxWorld/Framework/Input/InputController.h"

namespace NX
{

    CLASS();
    class BaseSpectator : public Actor
    {
        ECS_DECL(BaseSpectator, NX::Actor);

    public:
        [[nodiscard]] Core::StringAtom getCacheHash() const override;
        [[nodiscard]] InputController* getInputController();
        [[nodiscard]] const InputController* getInputController() const;

    protected:
        void onInitialize() override;
    };

    CLASS();
    class Spectator2D : public BaseSpectator
    {
        ECS_DECL(Spectator2D, NX::BaseSpectator);

    public:
        // TODO: Move it to Controller
        FIELD();
        float baseMlt = 2.f;
        FIELD();
        float leftShiftMlt = 8.f;
        FIELD();
        float minZoom = 0.15f;
        FIELD();
        float maxZoom = 5.f;

    protected:
        void onInitialize() override;
    };

    CLASS();
    class Spectator3D : public BaseSpectator
    {
        ECS_DECL(Spectator3D, NX::BaseSpectator);

    public:
        // TODO: Move it to Controller
        FIELD();
        float baseMlt = 2.f;
        FIELD();
        float leftShiftMlt = 8.f;
        FIELD();
        float leftCtrlMlt = 0.4f;

    protected:
        void onInitialize() override;
        void onTick(float delta) override;
    };

} // namespace NX

#include "Spectator.generated.h" // added by the code generator. Better don't move it.
