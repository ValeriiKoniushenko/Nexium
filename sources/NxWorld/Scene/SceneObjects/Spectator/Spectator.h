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
        // TODO: Move it to Controller
        FIELD();
        float speed = 600.f;

        FIELD();
        float mouseSensitivity = 0.5f;

        FIELD();
        float gamepadDeadzone = 0.15f;

    public:
        [[nodiscard]] Core::StringAtom getCacheHash() const override;
        [[nodiscard]] InputController* getInputController() const noexcept { return _input; }

    protected:
        void onInitialize() override;

        // Input is listened to only while the owning scene is current.
        void onSceneActivated() final;
        void onSceneDeactivated() final;
        virtual void setupInputActions(InputController& input) {}

    private:
        void updateInputControllerState();

        InputController* _input = nullptr;
        bool _isSceneActive = false;
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
        FIELD();
        float gamepadZoomSpeed = 30.f;

    protected:
        void setupInputActions(InputController& input) override;
        void onTick(float delta) override;

    private:
        void applyScroll(glm::vec2 offset, bool boost, bool zoom);
        void applyGamepad(const InputController& input, bool boost);
        bool applyZoom(float scrollStep);
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
        FIELD();
        float gamepadLookSpeed = 300.f;

    protected:
        void setupInputActions(InputController& input) override;
        void onTick(float delta) override;
    };

} // namespace NX

#include "Spectator.generated.h"
