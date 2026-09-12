// Nexium
// Copyright 2018-2027 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "Array.h"

namespace Core::Gui
{
    ECS_IMPL(ArrayCell);

    void ArrayCell::onInitialize()
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