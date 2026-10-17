// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "Core/String.h"
#include "JustReflectMe/Adapter.h"

#include <vector>

namespace NX
{
    CLASS();
    struct SceneTabsState
    {
        R_FRIEND(SceneTabsState);

        FIELD(R::Required);
        int formatVersion = 1;
        FIELD(R::Required);
        std::vector<Core::StringAtom> open;
        FIELD(R::Required);
        Core::StringAtom current;
    };
} // namespace NX

#include "SceneTabsState.generated.h"
