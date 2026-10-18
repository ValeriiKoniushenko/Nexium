// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "NxFundamental/ECS/BaseComponent.h"

namespace NX
{
    /// Common ECS lifecycle for controllers, independent of their source of commands.
    CLASS();
    class BaseController : public BaseComponent
    {
        ECS_DECL(BaseController, NX::BaseComponent);
    };
} // namespace NX

#include "BaseController.generated.h" // added by the code generator. Better don't move it.
