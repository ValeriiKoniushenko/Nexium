/*
 * MIT License
 *
 * Copyright (c) 2018-2025 Valerii Koniushenko
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#pragma once

#include "Widget.h"

namespace Core::Gui
{
    CLASS();
    class Layout : public Widget
    {
        R_FRIEND_DECL(Core::Gui::Layout, Core::Gui::Widget);
        ECS_COMPONENT_DECL(Layout, Widget);

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
