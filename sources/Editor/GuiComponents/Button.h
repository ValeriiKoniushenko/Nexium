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

#include "Core/Delegate.h"
#include "Widget.h"

namespace Core
{

    class Button : public Widget
    {
        ECS_REGISTER_NEW_COMPONENT(Button, Widget);

    public:
        void setButtonColor(const Color4& value);
        void resetButtonColor();
        [[nodiscard]] std::optional<Color4> getButtonColor() const;

        void setButtonHoverColor(const Color4& value);
        void resetButtonHoverColor();
        [[nodiscard]] std::optional<Color4> getButtonHoverColor() const;

        void setButtonActiveColor(const Color4& value);
        void resetButtonActiveColor();
        [[nodiscard]] std::optional<Color4> getButtonActiveColor() const;

        void setTextColor(const Color4& value);
        void resetTextColor();
        [[nodiscard]] std::optional<Color4> getTextColor() const;

        void setBorderColor(const Color4& value);
        void resetBorderColor();
        [[nodiscard]] std::optional<Color4> getBorderColor() const;

        void setBorderWidth(float value);
        void resetBorderWidth();
        [[nodiscard]] std::optional<float> getBorderWidth() const;

        void setText(const StringAtom& string);
        [[nodiscard]] const StringAtom& getText() const noexcept;

        void setWidth(float width) override;
        void resetWidth();

        void setHeight(float height) override;
        void resetHeight();

        [[nodiscard]] glm::vec2 getRealSize() const;

        [[nodiscard]] float getWidth() const override { return getRealSize().x; }
        [[nodiscard]] float getHeight() const override { return getRealSize().y; }

    public: // delegates
        /**
         * @brief will be called when clicked
         * @prarm pointer to "this" context. Context of clicked button.
         */
        Delegate<void(Button*)> onClick;

    protected:
        void onDraw() override;
        void onInitialize() override;

    protected:
        std::optional<Color4> _buttonColor;
        std::optional<Color4> _buttonHoverColor;
        std::optional<Color4> _buttonActiveColor;
        std::optional<Color4> _textColor;
        std::optional<Color4> _borderColor;
        std::optional<float> _borderWidth;

        glm::vec2 _textSize = {};
        glm::vec2 _size = {};
    };

} // namespace Core