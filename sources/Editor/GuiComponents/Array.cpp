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

#include "Array.h"

#include "Button.h"
#include "Input.h"
#include "Label.h"
#include "Misc/IconsFontAwesome.h"

namespace Core::Gui
{
    ECS_COMPONENT_IMPL(BaseArray)
    ECS_COMPONENT_IMPL(BaseArray::Cell)
    ECS_COMPONENT_IMPL(StringArray)

    BaseArray::Cell::Cell(const StringAtom& name)
        : HorizontalLayout(componentType, name)
    {
        if (name.isEmpty())
        {
            setComponentName("Cell"_atom);
        }
        setFlex(Flex::FlexWidth);
        setVerticalAlign(Align::Center);
        setHorizontalAlign(Align::Left);

        label = addChildComponent<Label>();

        content = addChildComponent<HorizontalLayout>();
        content->setFlex(Flex::FlexWidth);
        content->setHorizontalAlign(Align::SpaceBetween);
        content->setVerticalAlign(Align::Center);

        deleteButton = addChildComponent<Button>();
        deleteButton->setText(ICON_FA_TRASH);
    }

    void BaseArray::Cell::setIndexText(std::size_t i)
    {
        if (Verify(label))
        {
            label->setText(StringAtom::MakeFrom(i) + "#"_atom);
        }
    }

    void BaseArray::recreate()
    {
        _children.clear();

        for (std::size_t i = 0; i < getSize(); ++i)
        {
            auto* cell = addChildComponent<Cell>();
            cell->setIndexText(i);
            *cell->content = std::move(*getFilledContent(i));
            cell->content->setIsAutoDraw(false);
        }
    }

    void BaseArray::onInitialize()
    {
        VerticalLayout::onInitialize();

        setComponentName("Array"_atom);
        setVerticalAlign(Align::Top);
        setHorizontalAlign(Align::Center);

        recreate();
    }

    HorizontalLayout::Ptr StringArray::getFilledContent(std::size_t i)
    {
        auto l = HorizontalLayout::Create();

        auto label = l->addChildComponent<Label>();
        label->setText(_data[i]);

        return l;
    }
} // namespace Core::Gui