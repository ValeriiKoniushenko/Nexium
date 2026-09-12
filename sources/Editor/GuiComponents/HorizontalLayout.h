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
    class HorizontalLayout : public Layout
    {
        ECS_DECL_NO_CNSTR(HorizontalLayout, Core::Gui::Layout);

    public:
        explicit HorizontalLayout(const StringAtom& name = ""_atom);

        [[nodiscard]] float getWidth() const override;

        [[nodiscard]] float getHeight() const override;

        void setFitContent(bool value)
        {
            _fitContent = value;
            setFlex(Flex::Fixed);
        }

        [[nodiscard]] bool getFitContent() const noexcept { return _fitContent; }

    protected:
        void onAddChild(BaseComponent* newChild) override;

        void onTick(float delta) override;

        void onDraw() override;

        void onInitialize() override;

        void prepareAlignSpaceBetween();

        void prepareAlignLeft();

        void prepareAlignRight();

        void prepareAlignCenter();

        void calcYOffsets();

        void directDraw();

        void recalcFlexChildren();

    protected:
        std::vector<float> _yOffsets;

        // for space between
        FIELD();
        float _spacing = 0.f;

        FIELD();
        bool _fitContent = false;
    };
} // namespace Core::Gui

#include "HorizontalLayout.generated.h" // added by the code generator. Better don't move it.
