// Nexium
// Copyright 2018-2027 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "Layout.h"

namespace Core::Gui
{
    ECS_IMPL(Layout);

    void Layout::setHorizontalAlign(Align align)
    {
        _align = align;
    }
    Align Layout::getHorizontalAlign() const noexcept
    {
        return _align;
    }

    void Layout::setVerticalAlign(Align align)
    {
        _secondAlign = align;
    }
    Align Layout::getVerticalAlign() const noexcept
    {
        return _secondAlign;
    }

    void Layout::setHeight(float value)
    {
        _height = value;
    }
    void Layout::resetHeight()
    {
        _height.reset();
    }
    void Layout::setWidth(float value)
    {
        _width = value;
    }
    void Layout::resetWidth()
    {
        _width.reset();
    }

    void Layout::setPaddings(float left, float right, float top, float bottom)
    {
        _paddings = glm::vec4(left, right, top, bottom);
    }

    bool Layout::atLeastOne(Flex f)
    {
        for (auto&& child : _children)
        {
            if (child->isEnabled())
            {
                if ((static_cast<int>(child->unsafeCastTo<Widget>()->getFlex())
                     & static_cast<int>(f))
                    != 0)
                {
                    return true;
                }
            }
        }

        return false;
    }

    void Layout::onInitialize()
    {
        Widget::onInitialize();

        for (auto&& child : _children)
        {
            if (!child->isInitialized())
            {
                child->initialize();
            }
        }
    }
} // namespace Core::Gui