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

    class HorizontalLayout : public Layout
    {
        ECS_REGISTER_NEW_COMPONENT_NO_CNSTR(HorizontalLayout, Layout);

    public:
        explicit HorizontalLayout(const StringAtom& name = ""_atom);

        [[nodiscard]] float getWidth() const override;
        [[nodiscard]] float getHeight() const override;

        void setFitContent(bool value) { _fitContent = value; }
        [[nodiscard]] bool getFitContent() const noexcept { return _fitContent; }

    protected:
        void onAddChild(BaseComponent* newChild) override;
        void onTick(float delta) override;
        void onDraw() override;
        void onInitialize() override;

        void prepareAlignSpaceBetween();
        void prepareAlignLeft();
        void prepareAlignRight();
        void prepareAlignCenter();
        void calcYOffsets();
        void directDraw();
        void recalcFlexChildren();

    protected:
        std::vector<float> _yOffsets;

        // for space between
        float _spacing = 0.f;

        bool _fitContent = false;
    };

} // namespace Core