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

#include "NxFundamental/ECS/BaseComponent.h"

namespace NX
{
    CLASS();
    class InputController : public NX::BaseComponent
    {
        ECS_DECL(InputController, NX::BaseComponent);
    };
} // namespace NX

#include "InputController.generated.h" // added by the code generator. Better don't move it.
