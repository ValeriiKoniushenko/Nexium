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

#include "Button.h"

#include "ImGui/imgui.h"

namespace Core
{

    ECS_REGISTER_NEW_COMPONENT_TYPE(Button)

    void Button::setButtonColor(const Color4& value)
    {
        _overriddenButtonColor = value;
    }

    void Button::resetButtonColor()
    {
        _overriddenButtonColor.reset();
    }

    [[nodiscard]] std::optional<Color4> Button::getButtonColor() const
    {
        return _overriddenButtonColor;
    }

    void Button::setTextColor(const Color4& value)
    {
        _overriddenTextColor = value;
    }

    void Button::resetTextColor()
    {
        _overriddenTextColor.reset();
    }

    [[nodiscard]] std::optional<Color4> Button::getTextColor() const
    {
        return _overriddenTextColor;
    }

    void Button::setBorderColor(const Color4& value)
    {
        _overriddenBorderColor = value;
    }

    void Button::resetBorderColor()
    {
        _overriddenBorderColor.reset();
    }

    [[nodiscard]] std::optional<Color4> Button::getBorderColor() const
    {
        return _overriddenBorderColor;
    }

    void Button::setText(const StringAtom& string)
    {
        setComponentName(string);
        _textSize = ImGui::CalcTextSize(string.c_str());
    }

    const StringAtom& Button::getText() const noexcept
    {
        return getComponentName();
    }

    void Button::onDraw()
    {
        if (ImGui::Button(_name.c_str(), _size))
        {
            onClick.trigger();
        }
    }

    void Button::onInitialize()
    {
        Widget::onInitialize();
    }

} // namespace Core