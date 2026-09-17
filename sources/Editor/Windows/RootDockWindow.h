// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "BaseWindow.h"

namespace NX
{

    CLASS();
    class RootDockWindowEWC : public BaseEWC
    {
        ECS_DECL(RootDockWindowEWC, NX::BaseEWC);

    public:
        [[nodiscard]] std::vector<std::string> getCurrentWindowsList() const;

    protected:
        void onInitialize() override;

        void onDraw() override;

    protected:
        [[nodiscard]] bool beginWindowDraw() override;

        void endWindowDraw() override;
    };
} // namespace NX

#include "RootDockWindow.generated.h" // added by the code generator. Better don't move it.
