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

#include "ImGui/imgui_internal.h"

namespace Core::Gui
{

    ECS_COMPONENT_IMPL(Label);

    void Label::setWidth(float newWidth)
    {
        _width = newWidth;
        invalidateTextCache();
    }

    void Label::setHeight(float newHeight)
    {
        _height = newHeight;
    }

    void Label::setTextColor(const Color4& value)
    {
        _textColor = value;
    }

    void Label::resetTextColor()
    {
        _textColor.reset();
    }

    std::optional<Color4> Label::getTextColor() const
    {
        return _textColor;
    }

    void Label::setText(const StringAtom& string)
    {
        setComponentName(string);
        invalidateTextCache();
    }

    const StringAtom& Label::getText() const noexcept
    {
        return getComponentName();
    }

    glm::vec2 Label::getRealSize() const
    {
        auto out = _textSize;

        if (_width != -1.0f)
        {
            out.x = _width;
        }

        if (_height != -1.0f)
        {
            out.y = _height;
        }

        return out;
    }

    void Label::onDraw()
    {
        int pushedStyles = 0;

        pushedStyles += ImGui::OptPushStyleColor(ImGuiCol_Text, _textColor);

        const auto defaultCursor = ImGui::GetCursorPos();
        if (_width != -1.0f)
        {
            float offset = 0;
            if (_align.cast() == Align::Left)
            {
                // do nothing
            }
            else if (_align.cast() == Align::Center)
            {
                offset = (_width - _textSize.x) / 2.f;
            }
            else if (_align.cast() == Align::Right)
            {
                offset = _width - _textSize.x;
            }
            ImGui::SetCursorPosX(defaultCursor.x + offset);
        }

        ImGui::TextEx(_cachedText.c_str(), nullptr, ImGuiTextFlags_None);

        if (_width != -1.0f)
        {
            ImGui::SetCursorPosX(defaultCursor.x + _width);
            ImGui::Dummy(glm::vec2(0, 0));
        }

        ImGui::PopStyleColor(pushedStyles);
    }

    void Label::onInitialize()
    {
        Widget::onInitialize();
        if (_name.isEmpty())
        {
            // setText("Label"_atom);
        }

        if (_textSize.x == 0.f && _textSize.y == 0.f)
        {
            setText(getComponentName());
        }
    }

    void Label::invalidateTextCache()
    {
        auto originalSize = ImGui::CalcTextSize(_name.c_str());

        if (_isTruncateLongText && _width != -1.0f && originalSize.x > _width)
        {
            _cachedText.resize(0);

            _textSize = { _width, originalSize.y };
            const char* ellipsis = "..";
            float finalWidth = ImGui::CalcTextSize(ellipsis).x;

            for (auto c : _name)
            {
                finalWidth += ImGui::CalcTextSize(&c, &c + 1).x;
                _cachedText += c;
                if (finalWidth >= _width + style().ItemSpacing.x)
                {
                    break;
                }
            }
            _cachedText += ellipsis;
        }
        else
        {
            _textSize = originalSize;
            _cachedText = _name;
        }
    }
} // namespace Core::Gui