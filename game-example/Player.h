// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "NxWorld/Scene/SceneObjects/Rectangle/Rectangle.h"

CLASS();
class Player : public NX::SceneObj::RectangleAnimated
{
    ECS_DECL(Player, NX::SceneObj::RectangleAnimated);

protected:
    void onTick(float delta) override;

private:
    float _movementSpeed = 200.f;
};

#include "Player.generated.h" // added by the code generator. Better don't move it.
