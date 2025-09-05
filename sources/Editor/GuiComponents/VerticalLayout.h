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

#include "Layout.h"

namespace Core
{

    class VerticalLayout : public Layout<VerticalLayout>
    {
        ECS_REGISTER_NEW_COMPONENT(VerticalLayout, Layout<VerticalLayout>);

    public:
        [[nodiscard]] float getWidth() const override;
        [[nodiscard]] float getHeight() const override;

        /**
         * @brief It will work only while Top/Bottom/Center for vert. align
         */
        VerticalLayout& setSpacing(float value);
        VerticalLayout& resetSpacing();
        [[nodiscard]] float getSpacing() const;

    protected:
        void onAddChild(BaseComponent* newChild) override;
        void onTick(float delta) override;
        void onDraw() override;
        void onInitialize() override;

        void prepareAlignSpaceBetween();
        void prepareAlignTop();
        void prepareAlignBottom();
        void prepareAlignCenter();
        void directDraw();

        void calcXOffsets();

    protected:
        std::vector<float> _xOffsets;
        std::optional<float> _spacing;
    };

} // namespace Core