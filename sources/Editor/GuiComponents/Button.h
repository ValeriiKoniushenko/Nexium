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

#include "ImGui/imgui.h"
#include "Widget.h"

#include <Core/Color.h>
#include <Core/Delegate.h>

namespace Core
{

    class Button : public Widget
    {
        ECS_REGISTER_NEW_COMPONENT(Button, Widget);

    public:
        void setButtonColor(const Color4& value);
        void resetButtonColor();
        [[nodiscard]] std::optional<Color4> getButtonColor() const;

        void setTextColor(const Color4& value);
        void resetTextColor();
        [[nodiscard]] std::optional<Color4> getTextColor() const;

        void setBorderColor(const Color4& value);
        void resetBorderColor();
        [[nodiscard]] std::optional<Color4> getBorderColor() const;

        void setText(const StringAtom& string);
        [[nodiscard]] const StringAtom& getText() const noexcept;

    public: // delegates
        /**
         * @brief will be called when clicked
         */
        Delegate<void()> onClick;

    protected:
        void onDraw() override;
        void onInitialize() override;

    protected:
        std::optional<Color4> _overriddenButtonColor;
        std::optional<Color4> _overriddenTextColor;
        std::optional<Color4> _overriddenBorderColor;
        ImVec2 _textSize;
        ImVec2 _size;
    };

} // namespace Core