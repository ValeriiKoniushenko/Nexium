// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "HumanController.h"

#include <cstdlib>

namespace NX
{
    ECS_IMPL(HumanController);

    InputController& HumanController::getInput()
    {
        initialize();
        for (auto& child : getChildren())
        {
            if (auto* input = child->tryCastTo<InputController>())
            {
                return *input;
            }
        }
        return *addChildComponent<InputController>("Input"_atom, InputContext::Gameplay);
    }

    const InputController& HumanController::getInput() const
    {
        for (const auto& child : getChildren())
        {
            if (const auto* input = child->tryCastTo<InputController>())
            {
                return *input;
            }
        }
        Assert(false, "Initialize the controller before reading its input.");
        std::abort();
    }

    void HumanController::onInitialize()
    {
        GameController::onInitialize();
        (void)getInput();
    }
} // namespace NX
