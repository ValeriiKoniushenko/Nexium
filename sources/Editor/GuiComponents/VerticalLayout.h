// Nexium
// Copyright 2018-2027 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "Layout.h"

namespace Core::Gui
{
    CLASS();
    class VerticalLayout : public Layout
    {
        ECS_DECL_NO_CNSTR(VerticalLayout, Core::Gui::Layout);

    public:
        explicit VerticalLayout(const StringAtom& name = "");

        [[nodiscard]] float getWidth() const override;

        [[nodiscard]] float getHeight() const override;

        /// It will work only while Top/Bottom/Center for vert. align
        void setSpacing(float value);

        void resetSpacing();

        [[nodiscard]] float getSpacing() const;

    protected:
        void onAddChild(BaseComponent* newChild) override;

        void recalcFlexChildren();

        void onTick(float delta) override;

        void onDraw() override;

        void onInitialize() override;

        void prepareAlignSpaceBetween();

        void prepareAlignTop();

        void prepareAlignBottom();

        void prepareAlignCenter();

        void directDraw();

        void calcXOffsets();

    private:
        void recalcFlexWidthChildren();
        void recalcFlexHeightChildren();

    protected:
        std::vector<float> _xOffsets;

        FIELD();
        std::optional<float> _spacing;
    };
} // namespace Core::Gui

#include "VerticalLayout.generated.h" // added by the code generator. Better don't move it.
