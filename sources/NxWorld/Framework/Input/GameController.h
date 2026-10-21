// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "BaseController.h"
#include "NxWorld/Entities/Actor.h"

namespace NX
{
    /// Possesses a gameplay actor without owning its lifetime or choosing an input source.
    CLASS();
    class GameController : public BaseController
    {
        ECS_DECL(GameController, NX::BaseController);

    public:
        GameController(const GameController& other);
        GameController& operator=(const GameController&) = delete;
        GameController(GameController&&) = delete;
        GameController& operator=(GameController&&) = delete;
        ~GameController() override = default;

        // tag::input-game-controller-api[]
        void possess(Actor* actor);
        void unpossess();

        [[nodiscard]] Actor* getControlledActor() noexcept;
        [[nodiscard]] const Actor* getControlledActor() const noexcept;

        void clear() override;
        void onPreDeserialize(AbstractComponent* obj) override;

    protected:
        virtual void onPossess(Actor& actor);
        virtual void onUnpossess(Actor& actor);
        void onDeinitialize() override;
        // end::input-game-controller-api[]

    private:
        Core::WeakPtr<Actor> _controlledActor;
    };
} // namespace NX

#include "GameController.generated.h" // added by the code generator. Better don't move it.
