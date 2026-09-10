/*
 * MIT License
 *
 * Copyright (c) 2018-2027 Valerii Koniushenko
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#pragma once

#include "Widget.h"

namespace Core::Gui
{
    CLASS();
    class Label : public Widget
    {
        ECS_DECL(Label, Core::Gui::Widget);

    public:
        void setWidth(float newWidth) override;

        void setHeight(float newHeight) override;

        void setTextColor(const Color4& value);

        void resetTextColor();

        [[nodiscard]] std::optional<Color4> getTextColor() const;

        void setText(const StringAtom& string);

        [[nodiscard]] const StringAtom& getText() const noexcept;

        [[nodiscard]] glm::vec2 getRealSize() const;

        [[nodiscard]] float getWidth() const override { return getRealSize().x; }
        [[nodiscard]] float getHeight() const override { return getRealSize().y; }

        void setHorizontalAlign(Align align) { _align = align; }
        [[nodiscard]] Align getHorizontalAlign() const noexcept { return _align; }

        void setTruncateLongText(bool value) noexcept
        {
            _isTruncateLongText = value;
            invalidateTextCache();
        }

        [[nodiscard]] bool getTruncateLongText() const noexcept { return _isTruncateLongText; }

    protected:
        void onDraw() override;

        void onInitialize() override;

        void invalidateTextCache();

    protected:
        FIELD();
        std::optional<Core::Color4> _textColor;

        StringAtom _cachedText;

        FIELD();
        Core::Gui::Align _align = Core::Gui::Align::Left;

        FIELD();
        glm::vec2 _textSize = glm::vec2{};

        FIELD();
        float _width = -1.0f;

        FIELD();
        float _height = -1.0f;

        FIELD();
        bool _isTruncateLongText = true;
    };
} // namespace Core::Gui

#include "Label.generated.h" // added by the code generator. Better don't move it.
