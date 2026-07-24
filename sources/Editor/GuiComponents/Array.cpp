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

#include "Array.h"

namespace Core::Gui
{
    ECS_IMPL(ArrayCell) void ArrayCell::onInitialize()
    {
        HorizontalLayout::onInitialize();

        if (getComponentName().isEmpty())
        {
            setComponentName("ArrayCell"_atom);
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
    };
    HorizontalLayout::Ptr _StringArray_ArrayCellViewerFunc::operator()(const StringAtom& str) const
    {
        auto l = HorizontalLayout::Create();
        const auto label = l->addChildComponent<Label>();
        label->setFlex(Flex::FlexWidth);
        label->setText(str);
        return l;
    }

    StringAtom _StringArray_ViewFetchFunc::operator()(HorizontalLayout* layout) const
    {
        return layout->getFirstChildAs<Label>()->getText();
    }
} // namespace Core::Gui