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
    ECS_REGISTER_NEW_COMPONENT_TYPE(ToggleButton)

    void Button::setButtonColor(const Color4& value)
    {
        _buttonColor = value;
    }

    void Button::resetButtonColor()
    {
        _buttonColor.reset();
    }

    std::optional<Color4> Button::getButtonColor() const
    {
        return _buttonColor;
    }

    void Button::setButtonHoverColor(const Color4& value)
    {
        _buttonHoverColor = value;
    }

    void Button::resetButtonHoverColor()
    {
        _buttonHoverColor.reset();
    }

    std::optional<Color4> Button::getButtonHoverColor() const
    {
        return _buttonHoverColor;
    }

    void Button::setButtonActiveColor(const Color4& value)
    {
        _buttonActiveColor = value;
    }

    void Button::resetButtonActiveColor()
    {
        _buttonActiveColor.reset();
    }

    std::optional<Color4> Button::getButtonActiveColor() const
    {
        return _buttonActiveColor;
    }

    void Button::setTextColor(const Color4& value)
    {
        _textColor = value;
    }

    void Button::resetTextColor()
    {
        _textColor.reset();
    }

    std::optional<Color4> Button::getTextColor() const
    {
        return _textColor;
    }

    void Button::setBorderColor(const Color4& value)
    {
        _borderColor = value;
    }

    void Button::resetBorderColor()
    {
        _borderColor.reset();
    }

    std::optional<Color4> Button::getBorderColor() const
    {
        return _borderColor;
    }

    void Button::setBorderWidth(float value)
    {
        _borderWidth = value;
    }

    void Button::resetBorderWidth()
    {
        _borderWidth.reset();
    }

    std::optional<float> Button::getBorderWidth() const
    {
        return _borderWidth;
    }

    void Button::setText(const StringAtom& string)
    {
        setComponentName(string);
        _textSize = ImGui::CalcTextSize(string.c_str());
        _size = ImGui::CalcItemSize({}, _textSize.x + style().FramePadding.x * 2.0f,
                                    _textSize.y + style().FramePadding.y * 2.0f);
    }

    const StringAtom& Button::getText() const noexcept
    {
        return getComponentName();
    }

    void Button::setWidth(float width)
    {
        _size.x = width;
    }

    void Button::resetWidth()
    {
        _size.x = 0.0f;
    }

    void Button::setHeight(float height)
    {
        _size.y = height;
    }

    void Button::resetHeight()
    {
        _size.y = 0.0f;
    }

    glm::vec2 Button::getRealSize() const
    {
        return _size;
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
            onClick.trigger(this);
            onClickEvent();
        }

        ImGui::PopStyleColor(pushedStyles);
        ImGui::PopStyleVar(pushedVars);
    }

    void Button::onInitialize()
    {
        Widget::onInitialize();

        if (_name.isEmpty())
        {
            setText("Button"_atom);
        }
        if (_textSize.x == 0.0f && _textSize.y == 0.0f)
        {
            setText(getComponentName());
        }
        if (_size.x == 0.0f && _size.y == 0.0f)
        {
            _size = ImGui::CalcItemSize(_size, _textSize.x + style().FramePadding.x * 2.0f,
                                        _textSize.y + style().FramePadding.y * 2.0f);
        }
    }

    void ToggleButton::preDraw()
    {
        Button::preDraw();

        NormColor4 color = Color4(0, 0, 0, 0);
        if (_isActive)
        {
            if (_buttonColor)
            {
                color = NormColor4::From(*_buttonColor);
            }
            else
            {
                color = NormColor4(style().Colors[ImGuiCol_Button]);
            }
        }

        ImGui::PushStyleColor(ImGuiCol_Button, color);
    }

    void ToggleButton::postDraw()
    {
        Button::postDraw();
        ImGui::PopStyleColor();
    }

    void ToggleButton::onClickEvent()
    {
        _isActive = !_isActive;
        onToggle.trigger(this, _isActive);
    }

} // namespace Core