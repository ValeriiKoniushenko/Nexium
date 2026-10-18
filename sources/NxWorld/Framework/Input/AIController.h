// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "GameController.h"

namespace NX
{
    /// Extend onTick() to produce gameplay behavior without hardware input bindings.
    CLASS();
    class AIController : public GameController
    {
        ECS_DECL(AIController, NX::GameController);
    };
} // namespace NX

#include "AIController.generated.h" // added by the code generator. Better don't move it.
