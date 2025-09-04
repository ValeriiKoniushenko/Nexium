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
#include "ImGui/imgui_internal.h"

namespace Core
{

    ECS_REGISTER_NEW_COMPONENT_TYPE(Button)

    Button& Button::setButtonColor(const Color4& value)
    {
        _buttonColor = value;
        return *this;
    }

    Button& Button::resetButtonColor()
    {
        _buttonColor.reset();
        return *this;
    }

    std::optional<Color4> Button::getButtonColor() const
    {
        return _buttonColor;
    }

    Button& Button::setButtonHoverColor(const Color4& value)
    {
        _buttonHoverColor = value;
        return *this;
    }

    Button& Button::resetButtonHoverColor()
    {
        _buttonHoverColor.reset();
        return *this;
    }

    std::optional<Color4> Button::getButtonHoverColor() const
    {
        return _buttonHoverColor;
    }

    Button& Button::setButtonActiveColor(const Color4& value)
    {
        _buttonActiveColor = value;
        return *this;
    }

    Button& Button::resetButtonActiveColor()
    {
        _buttonActiveColor.reset();
        return *this;
    }

    std::optional<Color4> Button::getButtonActiveColor() const
    {
        return _buttonActiveColor;
    }

    Button& Button::setTextColor(const Color4& value)
    {
        _textColor = value;
        return *this;
    }

    Button& Button::resetTextColor()
    {
        _textColor.reset();
        return *this;
    }

    std::optional<Color4> Button::getTextColor() const
    {
        return _textColor;
    }

    Button& Button::setBorderColor(const Color4& value)
    {
        _borderColor = value;
        return *this;
    }

    Button& Button::resetBorderColor()
    {
        _borderColor.reset();
        return *this;
    }

    std::optional<Color4> Button::getBorderColor() const
    {
        return _borderColor;
    }

    Button& Button::setText(const StringAtom& string)
    {
        setComponentName(string);
        _textSize = ImGui::CalcTextSize(string.c_str());
        return *this;
    }

    const StringAtom& Button::getText() const noexcept
    {
        return getComponentName();
    }

    Button& Button::setWidth(float width)
    {
        _size.x = width;
        return *this;
    }

    Button& Button::resetWidth()
    {
        _size.x = 0.0f;
        return *this;
    }

    float Button::getWidth() const
    {
        return _size.x;
    }

    ImVec2 Button::getRealSize() const
    {
        return ImGui::CalcItemSize(_size, _textSize.x + style().FramePadding.x * 2.0f,
                                   _textSize.y + style().FramePadding.y * 2.0f);
    }

    void Button::onDraw()
    {
        int pushedStyles = 0;
        int pushedVars = 0;

        pushedStyles += ImGui::OptPushStyleColor(ImGuiCol_Button, _buttonColor);
        pushedStyles += ImGui::OptPushStyleColor(ImGuiCol_ButtonHovered, _buttonHoverColor);
        pushedStyles += ImGui::OptPushStyleColor(ImGuiCol_ButtonActive, _buttonActiveColor);
        pushedStyles += ImGui::OptPushStyleColor(ImGuiCol_Text, _textColor);
        pushedStyles += ImGui::OptPushStyleColor(ImGuiCol_Border, _borderColor);

        pushedVars += ImGui::OptPushStyleVar(ImGuiStyleVar_FrameBorderSize, _borderWidth);

        if (ImGui::ButtonEx(_name.c_str(), _size, ImGuiButtonFlags_None, &_textSize))
        {
            onClick.trigger();
        }

        ImGui::PopStyleColor(pushedStyles);
        ImGui::PopStyleVar(pushedVars);
    }

    void Button::onInitialize()
    {
        Widget::onInitialize();

        setText("Button");
    }

} // namespace Core