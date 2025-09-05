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

#include "Label.h"

namespace Core
{

    ECS_REGISTER_NEW_COMPONENT_TYPE(Label);

    Label& Label::setTextColor(const Color4& value)
    {
        _textColor = value;
        return *this;
    }

    Label& Label::resetTextColor()
    {
        _textColor.reset();
        return *this;
    }

    std::optional<Color4> Label::getTextColor() const
    {
        return _textColor;
    }

    Label& Label::setText(const StringAtom& string)
    {
        setComponentName(string);
        _textSize = ImGui::CalcTextSize(string.c_str());
        return *this;
    }

    const StringAtom& Label::getText() const noexcept
    {
        return getComponentName();
    }

    ImVec2 Label::getRealSize() const
    {
        return _textSize;
    }

    void Label::onDraw()
    {
        int pushedStyles = 0;

        pushedStyles += ImGui::OptPushStyleColor(ImGuiCol_Text, _textColor);

        ImGui::TextUnformatted(_name.c_str());

        ImGui::PopStyleColor(pushedStyles);
    }

    void Label::onInitialize()
    {
        Widget::onInitialize();
        if (_name.isEmpty())
        {
            setText("Label");
        }

        if (_textSize.x == 0.f && _textSize.y == 0.f)
        {
            setText(getComponentName());
        }
    }
} // namespace Core