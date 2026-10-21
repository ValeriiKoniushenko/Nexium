// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "GameController.h"

namespace NX
{
    ECS_IMPL(GameController);

    GameController::GameController(const GameController& other)
        : BaseController(other)
    {
        // Possession is runtime state and must not transfer to another controller.
    }

    void GameController::possess(Actor* actor)
    {
        if (_controlledActor.get() == actor)
        {
            return;
        }

        unpossess();
        _controlledActor = actor;
        if (actor)
        {
            onPossess(*actor);
        }
    }

    void GameController::unpossess()
    {
        auto actor = _controlledActor.tryLoad();
        _controlledActor.reset();
        if (actor)
        {
            onUnpossess(*actor);
        }
    }

    void GameController::clear()
    {
        unpossess();
        BaseController::clear();
    }

    void GameController::onPreDeserialize(AbstractComponent* obj)
    {
        unpossess();
        BaseController::onPreDeserialize(obj);
    }

    void GameController::onDeinitialize()
    {
        unpossess();
        BaseController::onDeinitialize();
    }

    Actor* GameController::getControlledActor() noexcept
    {
        return _controlledActor.get();
    }

    const Actor* GameController::getControlledActor() const noexcept
    {
        return _controlledActor.get();
    }

    void GameController::onPossess(Actor& /*actor*/)
    {
    }

    void GameController::onUnpossess(Actor& /*actor*/)
    {
    }
} // namespace NX
