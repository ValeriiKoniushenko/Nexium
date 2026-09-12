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
#include "assimp/postprocess.h"

#include <optional>
#include <vector>

namespace Assimp
{

    [[nodiscard]] Core::StringAtom aiPostProcessStepsToString(aiPostProcessSteps);

    [[nodiscard]] std::optional<aiPostProcessSteps> aiPostProcessStepsFromString(
        const Core::StringAtom& val);

    extern const std::vector<aiPostProcessSteps> aiPostProcessStepsAsVector;

} // namespace Assimp
