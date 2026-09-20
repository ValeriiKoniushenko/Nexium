// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "HorizontalLayout.h"

#include <algorithm>

namespace NX::Gui
{
    namespace
    {
        float measureContentWidth(const HorizontalLayout& layout)
        {
            float width = 0.f;
            bool first = true;
            for (const auto& child : layout.getChildren())
            {
                if (!child->isEnabled())
                {
                    continue;
                }
                if (!first)
                {
                    width += ImGui::GetStyle().ItemSpacing.x;
                }
                width += child->unsafeCastTo<Widget>()->getWidth();
                first = false;
            }
            return width;
        }
    } // namespace

    ECS_IMPL(HorizontalLayout);

    HorizontalLayout::HorizontalLayout(const StringAtom& name)
        : Layout(componentType, name)
    {
        if (name.isEmpty())
        {
            setComponentName("HorizontalLayout"_atom);
        }
        setFlex(Flex::FlexWidth);
        setVerticalAlign(Align::Center);
        setHorizontalAlign(Align::Left);
    }

    float HorizontalLayout::getWidth() const
    {
        if (!_fitContent)
        {
            if (_width)
            {
                return *_width;
            }
            if (!hasParent())
            {
                return ImGui::GetContentRegionAvail().x;
            }
        }

        return measureContentWidth(*this) + _paddings.x + _paddings.y;
    }

    float HorizontalLayout::getHeight() const
    {
        float height = _height.value_or(0.f);
        if (!_height)
        {
            for (const auto& child : _children)
            {
                if (child->isEnabled())
                {
                    height = std::max(height, child->unsafeCastTo<Widget>()->getHeight());
                }
            }
        }
        return height + _paddings.z + _paddings.w;
    }

    void HorizontalLayout::onAddChild(BaseComponent* newChild)
    {
        Widget::onAddChild(newChild);
        newChild->unsafeCastTo<Widget>()->setIsAutoDraw(false);
    }

    void HorizontalLayout::onTick(float delta)
    {
        Widget::onTick(delta);
    }

    void HorizontalLayout::onDraw()
    {
        const auto originalCursor = ImGui::GetCursorPos();
        const auto width = getWidth();
        recalcFlexChildren();

        calcYOffsets();

        if (_align == Align::Left)
        {
            prepareAlignLeft();
        }
        else if (_align == Align::Right)
        {
            prepareAlignRight();
        }
        else if (_align == Align::SpaceBetween)
        {
            prepareAlignSpaceBetween();
        }
        else if (_align == Align::Center)
        {
            prepareAlignCenter();
        }
        else
        {
            Assert(false);
            prepareAlignLeft();
        }

        directDraw();

        ImGui::SetCursorPos(originalCursor);
        ImGui::Dummy(glm::vec2(width, getHeight()));
    }

    void HorizontalLayout::onInitialize()
    {
        Layout::onInitialize();
    }

    void HorizontalLayout::prepareAlignSpaceBetween()
    {
        _spacing = style().ItemSpacing.x;
        if (!atLeastOne(Flex::FlexWidth))
        {
            const auto count = std::ranges::count_if(_children, [](const auto& child)
                                                     { return child->isEnabled(); });
            if (count > 1)
            {
                const auto remaining
                    = getWidth() - _paddings.x - _paddings.y - measureContentWidth(*this);
                _spacing += std::max(0.f, remaining) / static_cast<float>(count - 1);
            }
        }
    }

    void HorizontalLayout::prepareAlignLeft()
    {
        // do nothing
    }

    void HorizontalLayout::prepareAlignRight()
    {
        if (hasChildren() && !atLeastOne(Flex::FlexWidth))
        {
            const auto remaining
                = getWidth() - _paddings.x - _paddings.y - measureContentWidth(*this);
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + std::max(0.f, remaining));
        }
    }

    void HorizontalLayout::prepareAlignCenter()
    {
        if (hasChildren() && !atLeastOne(Flex::FlexWidth))
        {
            const auto remaining
                = getWidth() - _paddings.x - _paddings.y - measureContentWidth(*this);
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + std::max(0.f, remaining) / 2.f);
        }
    }

    void HorizontalLayout::calcYOffsets()
    {
        const auto ownHeight = getHeight() - _paddings.z - _paddings.w;

        _yOffsets.resize(getChildrenCount());
        std::size_t i = 0;
        for (auto&& child : _children)
        {
            if (!child->isEnabled())
            {
                continue;
            }
            const auto w = child->unsafeCastTo<Widget>();
            if (_secondAlign == Align::Top)
            {
                _yOffsets.at(i) = _paddings.z;
            }
            else if (_secondAlign == Align::Bottom)
            {
                _yOffsets.at(i) = _paddings.z + ownHeight - w->getHeight();
            }
            else if (_secondAlign == Align::Center)
            {
                _yOffsets.at(i) = _paddings.z + (ownHeight - w->getHeight()) / 2.f;
            }
            ++i;
        }

        Assert(_yOffsets.size() == _children.size());
    }

    void HorizontalLayout::directDraw()
    {
        const auto origin = ImGui::GetCursorPos();
        const auto spacing = _align == Align::SpaceBetween ? _spacing : style().ItemSpacing.x;
        float x = origin.x + _paddings.x;
        std::size_t i = 0;

        for (auto& child : _children)
        {
            if (!child->isEnabled())
            {
                continue;
            }
            if (i != 0)
            {
                x += spacing;
            }
            ImGui::SetCursorPos(glm::vec2(x, origin.y + _yOffsets.at(i)));
            auto* widget = child->unsafeCastTo<Widget>();
            const auto width = widget->getWidth();
            widget->unhandledDraw();
            x += width;
            ++i;
        }
    }

    void HorizontalLayout::recalcFlexChildren()
    {
        float width = getWidth() - _paddings.x - _paddings.y;
        std::size_t count = 0;
        std::size_t flexCount = 0;
        for (auto& child : _children)
        {
            if (!child->isEnabled())
            {
                continue;
            }
            ++count;
            const auto* widget = child->unsafeCastTo<Widget>();
            if ((static_cast<int>(widget->getFlex()) & static_cast<int>(Flex::FlexWidth)) != 0)
            {
                ++flexCount;
            }
            else
            {
                width -= widget->getWidth();
            }
        }
        if (flexCount == 0)
        {
            return;
        }

        width -= style().ItemSpacing.x * static_cast<float>(count - 1);
        const auto childWidth = std::max(0.f, width / static_cast<float>(flexCount));
        for (auto& child : _children)
        {
            if (!child->isEnabled())
            {
                continue;
            }
            auto* widget = child->unsafeCastTo<Widget>();
            if ((static_cast<int>(widget->getFlex()) & static_cast<int>(Flex::FlexWidth)) != 0)
            {
                widget->setWidth(childWidth);
            }
        }
    }
} // namespace NX::Gui
