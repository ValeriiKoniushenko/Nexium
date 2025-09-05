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

namespace Core
{

    template<class DerivedLayout>
    class Layout : public Widget
    {
        ECS_REGISTER_NEW_COMPONENT(Layout, Widget);

    public:
        // clang-format off
        CreateEnum(Align, int,
            Left,           // For horizontal align
            Right,          // For horizontal align
            SpaceBetween,   // For horizontal align
            Center,         // For horizontal & vertical align
            Top,            // For vertical align
            Bottom          // For vertical align
        );
        // clang-format on

    public:
        DerivedLayout& setHorizontalAlign(Align align)
        {
            _align = align;
            return *static_cast<DerivedLayout*>(this);
        }
        [[nodiscard]] Align getHorizontalAlign() const noexcept { return _align; }

        DerivedLayout& setVerticalAlign(Align align)
        {
            _secondAlign = align;
            return *static_cast<DerivedLayout*>(this);
        }
        [[nodiscard]] Align getVerticalAlign() const noexcept { return _secondAlign; }

        DerivedLayout& setHeight(float value)
        {
            _height = value;
            return *static_cast<DerivedLayout*>(this);
        }
        DerivedLayout& resetHeight()
        {
            _height.reset();
            return *static_cast<DerivedLayout*>(this);
        }
        DerivedLayout& setWidth(float value)
        {
            _width = value;
            return *static_cast<DerivedLayout*>(this);
        }
        DerivedLayout& resetWidth()
        {
            _width.reset();
            return *static_cast<DerivedLayout*>(this);
        }

    protected:
        std::optional<float> _height;
        std::optional<float> _width;

        Align _align = Align::Left;
        Align _secondAlign = Align::Center;
    };

} // namespace Core