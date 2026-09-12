// Nexium
// Copyright 2018-2027 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "Core/Delegate.h"
#include "NxSubsystems/AssetsManager/TextureAsset.h"
#include "Widget.h"

namespace Core::Gui
{
    CLASS();
    class Button : public Widget
    {
        ECS_DECL(Button, Core::Gui::Widget);

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
        Delegate<void()>::Ptr onClick = Delegate<void()>::Create();

    protected:
        void onDraw() final;

        virtual void onButtonDraw();

        void onInitialize() override;

        virtual void onClickEvent() {}

    protected:
        FIELD();
        std::optional<Color4> _buttonColor;
        FIELD();
        std::optional<Color4> _buttonHoverColor;
        FIELD();
        std::optional<Color4> _buttonActiveColor;
        FIELD();
        std::optional<Color4> _textColor;
        FIELD();
        std::optional<Color4> _borderColor;
        FIELD();
        std::optional<float> _borderRound;
        FIELD();
        std::optional<float> _borderWidth;

        FIELD();
        glm::vec2 _textSize = glm::vec2{};
        FIELD();
        glm::vec2 _size = glm::vec2{};
        FIELD();
        glm::vec2 _minSize = glm::vec2{};
    };

    CLASS();
    class ToggleButton : public Button
    {
        ECS_DECL(ToggleButton, Core::Gui::Button);

    public: // Delegates
        /// will be called when toggled
        /// @prarm bool true - if it becomes active; false - in another case
        Delegate<void(bool)>::Ptr onToggle = Delegate<void(bool)>::Create();

        void setActive(bool value) noexcept { _isActive = value; }
        [[nodiscard]] bool isActive() const noexcept { return _isActive; }

    protected:
        void preDraw() override;

        void postDraw() override;

        void onClickEvent() override;

    protected:
        FIELD();
        bool _isActive = true;
    };

    CLASS();
    class ImageButton : public Button
    {
        ECS_DECL(ImageButton, Core::Gui::Button);

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
        FIELD();
        std::optional<glm::vec2> _paddingSize;
        NXTexture _texture;
    };
} // namespace Core::Gui

#include "Button.generated.h" // added by the code generator. Better don't move it.
