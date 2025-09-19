// MIT License
//
// Copyright (c) 2019-2025 Valerii Koniushenko
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include "Widget.h"

namespace Core::Gui
{

    class Layout : public Widget
    {
        ECS_COMPONENT_DECL(Layout, Widget);

    public:
        void setHorizontalAlign(Align align) { _align = align; }
        [[nodiscard]] Align getHorizontalAlign() const noexcept { return _align; }

        void setVerticalAlign(Align align) { _secondAlign = align; }
        [[nodiscard]] Align getVerticalAlign() const noexcept { return _secondAlign; }

        void setHeight(float value) override { _height = value; }
        void resetHeight() { _height.reset(); }
        void setWidth(float value) override { _width = value; }
        void resetWidth() { _width.reset(); }

        void setPaddings(float left, float right, float top = 0, float bottom = 0)
        {
            _paddings = glm::vec4(left, right, top, bottom);
        }
        void setPaddings(const glm::vec4& lrtb) { _paddings = lrtb; }
        [[nodiscard]] const glm::vec4& getPaddings() const noexcept { return _paddings; }

    protected:
        [[nodiscard]] bool atLeastOne(Flex f);
        void onInitialize() override;

    protected:
        std::optional<float> _height;
        std::optional<float> _width;
        // Left Right Top Bottom
        glm::vec4 _paddings = {};

        Align _align = Align::None;
        Align _secondAlign = Align::None;
    };

} // namespace Core::Gui