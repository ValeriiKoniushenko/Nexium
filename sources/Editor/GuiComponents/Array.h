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

#include "HorizontalLayout.h"
#include "VerticalLayout.h"

namespace Core::Gui
{
    class Button;
    class Label;

    class BaseArray : public VerticalLayout
    {
        ECS_COMPONENT_DECL(BaseArray, VerticalLayout);

    public:
        class Cell : public HorizontalLayout
        {
            ECS_COMPONENT_DECL_NO_CNSTR(Cell, HorizontalLayout);

        public:
            Label* label = nullptr;
            HorizontalLayout* content = nullptr;
            Button* deleteButton = nullptr;

        public:
            explicit Cell(const StringAtom& name = "");

            void setIndexText(std::size_t i);
        };

    public:
        [[nodiscard]] virtual std::size_t getSize() const = 0;

    protected:
        [[nodiscard]] virtual HorizontalLayout::Ptr getFilledContent(std::size_t i) = 0;
        void recreate();

        void onInitialize() override;
    };

    class StringArray : public BaseArray
    {
        ECS_COMPONENT_DECL(StringArray, BaseArray);

    public:
        std::size_t getSize() const override { return _data.size(); }

    protected:
        [[nodiscard]] HorizontalLayout::Ptr getFilledContent(std::size_t i) override;

    protected:
        std::vector<StringAtom> _data = { "Hello", "World" };
    };

} // namespace Core::Gui