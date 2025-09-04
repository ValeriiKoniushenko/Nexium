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

#include "VerticalLayout.h"

namespace Core
{

    VerticalLayout& VerticalLayout::setSpacing(float value)
    {
        _spacing = value;
        return *this;
    }

    VerticalLayout& VerticalLayout::resetSpacing()
    {
        _spacing.reset();
        return *this;
    }

    float VerticalLayout::getSpacing() const
    {
        return _spacing.value_or(style().ItemSpacing.y);
    }

    void VerticalLayout::onTick(float delta)
    {
        int pushed = 0;

        if (_spacing)
        {
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing,
                                ImVec2(style().ItemSpacing.x, *_spacing));
            ++pushed;
        }

        Widget::onTick(delta);

        ImGui::PopStyleVar(pushed);
    }

    void VerticalLayout::onDraw()
    {
    }

    void VerticalLayout::onInitialize()
    {
        Widget::onInitialize();
    }

} // namespace Core