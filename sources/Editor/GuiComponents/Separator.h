// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "Widget.h"

namespace Core::Gui
{
    CLASS();
    class Separator : public Widget
    {
        ECS_DECL(Separator, Core::Gui::Widget);

    public:
        [[nodiscard]] float getWidth() const override { return -1; }
        [[nodiscard]] float getHeight() const override { return 1; }
        void setWidth(float) override { Assert(false, "Impossible to set size for Separator"); }
        void setHeight(float) override { Assert(false, "Impossible to set size for Separator"); }

    protected:
        void onDraw() override;
    };
} // namespace Core::Gui

#include "Separator.generated.h" // added by the code generator. Better don't move it.
