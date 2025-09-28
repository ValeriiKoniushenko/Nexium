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

#include "HorizontalLayout.h"

#include "ImGui/imgui_internal.h"

namespace Core::Gui
{
    ECS_COMPONENT_IMPL(HorizontalLayout)

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
        if (_fitContent)
        {
            if (!hasChildren())
            {
                return 0;
            }

            const float defaultSpacing = style().ItemSpacing.x;
            float width = 0.f;
            for (auto& child : _children)
            {
                if (!child->isEnabled())
                {
                    continue;
                }
                width += child->unsafeCastTo<Widget>()->getWidth();
                width += defaultSpacing;
            }
            width -= defaultSpacing;
            return width;
        }

        if (_width)
        {
            return *_width;
        }

        if (!hasParent())
        {
            return ImGui::GetContentRegionAvail().x;
        }

        float width = 0.f;
        // std::size_t fixedCount = 0;
        std::size_t flexWidthCount = 0;
        std::size_t total = 0;
        for (auto&& child : _children)
        {
            if (child->isEnabled())
            {
                auto* w = child->unsafeCastTo<Widget>();
                width += w->getWidth();
                ++total;
                if (w->getFlex().cast() == Flex::Fixed)
                {
                    // ++fixedCount;
                }
                else
                {
                    ++flexWidthCount;
                }
            }
        }

        if (total == flexWidthCount)
        {
            return width;
        }

        if (total > 1)
        {
            width += style().ItemSpacing.x * static_cast<float>(total - 1);
        }

        return width;
    }

    float HorizontalLayout::getHeight() const
    {
        if (_height)
        {
            return *_height + (_paddings.z + _paddings.w);
        }

        auto cmp = [](const BaseComponent::Ptr& a, const BaseComponent::Ptr& b)
        {
            return a->unsafeCastTo<Widget>()->getHeight() < b->unsafeCastTo<Widget>()->getHeight();
        };
        const auto maxHeightEl = std::ranges::max_element(_children, cmp);
        if (maxHeightEl == _children.end())
        {
            return 0;
        }

        const auto maxHeight = (*maxHeightEl)->unsafeCastTo<Widget>()->getHeight();
        return maxHeight + (_paddings.z + _paddings.w);
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
        recalcFlexChildren();

        const auto originalCursor = ImGui::GetCursorPos();

        calcYOffsets();

        if (_align.cast() == Align::Left)
        {
            prepareAlignLeft();
        }
        else if (_align.cast() == Align::Right)
        {
            prepareAlignRight();
        }
        else if (_align.cast() == Align::SpaceBetween)
        {
            prepareAlignSpaceBetween();
        }
        else if (_align.cast() == Align::Center)
        {
            prepareAlignCenter();
        }
        else
        {
            DEBUG_ASSERT(false);
            prepareAlignLeft();
        }

        directDraw();

        if (_width)
        {
            ImGui::SetCursorPosX(originalCursor.x + *_width - style().ItemSpacing.x);
        }

        ImGui::SetCursorPosY(originalCursor.y + getHeight() + style().ItemSpacing.y);

        if (!hasParent())
        {
            ImGui::SetCursorPosX(originalCursor.x - style().ItemSpacing.x);
        }
    }

    void HorizontalLayout::onInitialize()
    {
        Layout::onInitialize();
    }

    void HorizontalLayout::prepareAlignSpaceBetween()
    {
        if (hasChildren() && !atLeastOne(Flex::FlexWidth))
        {
            std::size_t i = 0;
            _spacing = getWidth();
            for (const auto& child : _children)
            {
                if (!child->isEnabled())
                {
                    continue;
                }
                _spacing -= child->unsafeCastTo<Widget>()->getWidth();
                ++i;
            }

            _spacing /= static_cast<float>(i <= 1 ? 1 : i - 1);
        }
        else
        {
            _spacing = 0;
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
            float spacing = getWidth();
            for (const auto& child : _children)
            {
                if (!child->isEnabled())
                {
                    continue;
                }
                spacing -= child->unsafeCastTo<Widget>()->getWidth();
                spacing -= style().ItemSpacing.x;
            }

            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + spacing);
        }
    }

    void HorizontalLayout::prepareAlignCenter()
    {
        if (hasChildren() && !atLeastOne(Flex::FlexWidth))
        {
            float spacing = getWidth();
            for (const auto& child : _children)
            {
                if (!child->isEnabled())
                {
                    continue;
                }
                spacing -= child->unsafeCastTo<Widget>()->getWidth();
                spacing -= style().ItemSpacing.x;
            }
            spacing -= style().ItemSpacing.x;
            spacing /= 2.f;

            ImGui::Dummy(glm::vec2(spacing, 0));
            ImGui::SameLine();
        }
    }

    void HorizontalLayout::calcYOffsets()
    {
        const auto ownHeight = getHeight();

        _yOffsets.resize(getChildrenCount());
        std::size_t i = 0;
        for (auto&& child : _children)
        {
            if (!child->isEnabled())
            {
                continue;
            }
            const auto w = child->unsafeCastTo<Widget>();
            if (_secondAlign.cast() == Align::Top)
            {
                _yOffsets.at(i) = 0;
            }
            else if (_secondAlign.cast() == Align::Bottom)
            {
                _yOffsets.at(i) = ownHeight - w->getHeight();
            }
            else if (_secondAlign.cast() == Align::Center)
            {
                _yOffsets.at(i) = (ownHeight - w->getHeight()) / 2.f;
            }
            ++i;
        }

        DEBUG_ASSERT(_yOffsets.size() == _children.size());
    }

    void HorizontalLayout::directDraw()
    {
        const auto originalYCursor = ImGui::GetCursorPosY();
        const auto defaultSpacing = style().ItemSpacing.x;
        std::size_t i = 0;

        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + _paddings.x);

        for (auto&& child : _children)
        {
            if (!child->isEnabled())
            {
                continue;
            }
            auto start = ImGui::GetCursorPosX();
            ImGui::SetCursorPosY(originalYCursor + _yOffsets.at(i));
            child->unsafeCastTo<Widget>()->unhandledDraw();

            const auto childWidth = child->unsafeCastTo<Widget>()->getWidth();
            auto finalOffset = start + childWidth;

            if (i != _children.size() - 1)
            {
                if (_align.cast() == Align::SpaceBetween)
                {
                    finalOffset += _spacing;
                }
                else
                {
                    finalOffset += defaultSpacing;
                }
            }
            ImGui::SetCursorPosX(finalOffset);

            ++i;
        }
    }

    void HorizontalLayout::recalcFlexChildren()
    {
        if (atLeastOne(Flex::FlexWidth))
        {
            const float defaultSpacing = style().ItemSpacing.x;
            float width = getWidth() - (_paddings.x + _paddings.y);

            int fixedCount = 0;
            int flexWidthCount = 0;
            for (auto&& child : _children)
            {
                if (!child->isEnabled())
                {
                    continue;
                }
                const auto w = child->unsafeCastTo<Widget>();
                const auto type = w->getFlex().cast();
                if (type == Flex::Fixed)
                {
                    width -= w->getWidth();
                    width -= defaultSpacing;
                    ++fixedCount;
                }
                else if (type == Flex::FlexWidth)
                {
                    ++flexWidthCount;
                }
            }
            if (fixedCount != 0)
            {
                width += defaultSpacing;
            }

            int deCounter = flexWidthCount;
            for (auto& child : _children)
            {
                if (!child->isEnabled())
                {
                    continue;
                }
                auto w = child->unsafeCastTo<Widget>();
                if (w->getFlex().cast() == Flex::FlexWidth)
                {
                    const float gap = deCounter != 0 ? defaultSpacing : 0;
                    const float finalWidth
                        = std::max(0.f, width / static_cast<float>(flexWidthCount) - gap);
                    w->setWidth(finalWidth);
                    --deCounter;
                }
                if (deCounter == 0)
                {
                    break;
                }
            }
        }
    }
} // namespace Core::Gui