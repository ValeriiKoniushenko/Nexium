// Nexium
// Copyright 2018-2027 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "BaseWindow.h"

namespace Core
{
    CLASS();
    class GameViewportEWC : public BaseFloatEWC
    {
        ECS_DECL(GameViewportEWC, Core::BaseFloatEWC);

    public:
        [[nodiscard]] const char* getIcon() override;

    protected:
        void onInitialize() override;
        void onUpdate() override;
        void onDraw() override;
    };
} // namespace Core

#include "GameViewport.generated.h" // added by the code generator. Better don't move it.
