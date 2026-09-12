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
    class Spacer : public Widget
    {
        ECS_DECL(Spacer, Core::Gui::Widget);

    public:
        [[nodiscard]] glm::vec2 getRealSize() const;

        [[nodiscard]] float getWidth() const override { return getRealSize().x; }
        [[nodiscard]] float getHeight() const override { return getRealSize().y; }

        void setWidth(float newWidth) override;

        void setHeight(float newHeight) override;

        void scaleCurrentWidth(float mlt);

        void scaleCurrentHeight(float mlt);

    protected:
        void onDraw() override;

        void onInitialize() override;

    protected:
        FIELD();
        glm::vec2 _size = glm::vec2{};
    };
} // namespace Core::Gui

#include "Spacer.generated.h" // added by the code generator. Better don't move it.
