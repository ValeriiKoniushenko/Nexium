// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "HorizontalLayout.h"
#include "Label.h"

namespace Core::Gui
{

    template<class /*IsWidget*/ T>
    class LabelRow : public HorizontalLayout
    {
        ECS_TEMPLATE_COMPONENT_DECL_NO_CNSTR(LabelRow, HorizontalLayout, T);

    public:
        explicit LabelRow(StringAtom labelText = ""_atom, float labelWidth = 0.f,
                          Align hAlign = Align::SpaceBetween)
            : HorizontalLayout(componentType, labelText),
              label(addChildComponent<Label>())
        {
            label->setText(labelText);
            if (labelWidth != 0.f)
            {
                label->setWidth(labelWidth);
            }

            input = addChildComponent<T>();

            setHorizontalAlign(hAlign);
            setVerticalAlign(Align::Center);
            setFlex(Flex::FlexWidth);

            labelText.pushFront("LabelRow-");
            setComponentName(labelText);
        }

        union
        {
            T* second = nullptr;
            T* input;
        };

        Label* label = nullptr;
    };

    ECS_TEMPLATE_COMPONENT_IMPL(LabelRow<T>, class T);

} // namespace Core::Gui
