/*
 * MIT License
 *
 * Copyright (c) 2018-2025 Valerii Koniushenko
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

#include "AssetsManager/TextureAsset.h"
#include "Core/Delegate.h"
#include "Graphics/Texture.h"
#include "Widget.h"

namespace Core::Gui
{

    class Button : public Widget
    {
        ECS_COMPONENT_DECL(Button, Widget);

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

        void setMinWidth(float width) noexcept;
        [[nodiscard]] float getMinWidth() const noexcept { return _minSize.x; }
        void setWidth(float width) override;
        void resetWidth();

        void setMinHeight(float height) noexcept;
        [[nodiscard]] float getMinHeight() const noexcept { return _minSize.y; }
        void setHeight(float height) override;
        void resetHeight();

        void setSize(glm::vec2 size) noexcept;
        void setMinSize(glm::vec2 size) noexcept;

        [[nodiscard]] glm::vec2 getRealSize() const;

        [[nodiscard]] float getWidth() const override { return getRealSize().x; }
        [[nodiscard]] float getHeight() const override { return getRealSize().y; }

        void setBorderRound(float value);
        void resetBorderRound();
        [[nodiscard]] std::optional<float> getBorderRound() const;

    public: // delegates
        Delegate<void()> onClick;

    protected:
        void onDraw() final;
        virtual void onButtonDraw();
        void onInitialize() override;

        virtual void onClickEvent() {}

    protected:
        std::optional<Color4> _buttonColor;
        std::optional<Color4> _buttonHoverColor;
        std::optional<Color4> _buttonActiveColor;
        std::optional<Color4> _textColor;
        std::optional<Color4> _borderColor;
        std::optional<float> _borderRound;
        std::optional<float> _borderWidth;

        glm::vec2 _textSize = {};
        glm::vec2 _size = {};
        glm::vec2 _minSize = {};
    };

    class ToggleButton : public Button
    {
        ECS_COMPONENT_DECL(ToggleButton, Button);

    public: // Delegates
        /**
         * @brief will be called when toggled
         * @prarm bool true - if it becomes active; false - in another case
         */
        Delegate<void(bool)> onToggle;

        void setActive(bool value) noexcept { _isActive = value; }
        [[nodiscard]] bool isActive() const noexcept { return _isActive; }

    protected:
        void preDraw() override;
        void postDraw() override;
        void onClickEvent() override;

    protected:
        bool _isActive = true;
    };

    class ImageButton : public Button
    {
        ECS_COMPONENT_DECL(ImageButton, Button);

    public:
        void setImage(const NXTexture& texture) { _texture = texture; }
        [[nodiscard]] const NXTexture& getImage() const noexcept { return _texture; }

        void setPaddingSize(glm::vec2 value) { _paddingSize = value; }
        [[nodiscard]] std::optional<glm::vec2> getPaddingSize() const { return _paddingSize; }

    protected:
        void preDraw() override;
        void onButtonDraw() override;
        void postDraw() override;

    protected:
        std::optional<glm::vec2> _paddingSize;
        NXTexture _texture;
    };

} // namespace Core::Gui