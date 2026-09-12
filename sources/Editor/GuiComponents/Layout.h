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
    class Layout : public Widget
    {
        ECS_DECL(Layout, Core::Gui::Widget);

    public:
        void setHorizontalAlign(Align align);
        [[nodiscard]] Align getHorizontalAlign() const noexcept;

        void setVerticalAlign(Align align);
        [[nodiscard]] Align getVerticalAlign() const noexcept;

        void setHeight(float value) override;
        void resetHeight();
        void setWidth(float value) override;
        void resetWidth();

        void setPaddings(float left, float right, float top = 0, float bottom = 0);

        void setPaddings(const glm::vec4& lrtb) { _paddings = lrtb; }
        [[nodiscard]] const glm::vec4& getPaddings() const noexcept { return _paddings; }

    protected:
        [[nodiscard]] bool atLeastOne(Flex f);

        void onInitialize() override;

    protected:
        FIELD();
        std::optional<float> _height;

        FIELD();
        std::optional<float> _width;

        // Left Right Top Bottom
        FIELD();
        glm::vec4 _paddings = glm::vec4{};

        FIELD();
        Core::Gui::Align _align = Core::Gui::Align::None;

        FIELD();
        Core::Gui::Align _secondAlign = Core::Gui::Align::None;
    };
} // namespace Core::Gui

#include "Layout.generated.h" // added by the code generator. Better don't move it.
