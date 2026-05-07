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

#include "HorizontalLayout.h"
#include "Label.h"

namespace Core::Gui
{

    template<IsWidget T>
    class LabelRow : public HorizontalLayout
    {
        ECS_TEMPLATE_COMPONENT_DECL_NO_CNSTR(LabelRow, HorizontalLayout, T);

    public:
        explicit LabelRow(StringAtom labelText = ""_atom, float labelWidth = 0.f,
                          Align hAlign = Align::SpaceBetween)
            : HorizontalLayout(componentType, labelText)
        {
            label = addChildComponent<Label>();
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

    ECS_TEMPLATE_COMPONENT_IMPL(LabelRow<T>, IsWidget T);

} // namespace Core::Gui

#include "LabelRow.generated.inl" // added by the code generator. Better don't move it.
