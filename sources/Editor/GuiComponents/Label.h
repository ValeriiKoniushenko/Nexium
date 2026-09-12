// Nexium
// Copyright 2018-2027 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

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
