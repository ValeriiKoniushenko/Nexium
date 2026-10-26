// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

// Module: Nexium_Tests

#pragma once

#include "NxFundamental/ECS/BaseComponent.h"

#include <functional>

// Runs `action` from its tick, like game code reacting during the scene update.
CLASS();
class TickActionComponent : public NX::BaseComponent
{
    ECS_DECL(TickActionComponent, NX::BaseComponent);

public:
    std::function<void()> action;

protected:
    void onTick(float /*delta*/) override
    {
        if (action)
        {
            action();
        }
    }
};

#include "TickActionComponent.generated.h" // added by the code generator. Better don't move it.
