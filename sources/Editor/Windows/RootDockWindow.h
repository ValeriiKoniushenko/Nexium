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
    class RootDockWindowEWC : public BaseEWC
    {
        ECS_DECL(RootDockWindowEWC, Core::BaseEWC);

    public:
        [[nodiscard]] std::vector<std::string> getCurrentWindowsList() const;

    protected:
        void onInitialize() override;

        void onDraw() override;

    protected:
        [[nodiscard]] bool beginWindowDraw() override;

        void endWindowDraw() override;
    };
} // namespace Core

#include "RootDockWindow.generated.h" // added by the code generator. Better don't move it.
