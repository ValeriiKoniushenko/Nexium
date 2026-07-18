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

#include "VerticalLayout.h"

namespace Core::Gui
{
    R_FRIEND_IMPL(VerticalLayout);
    ECS_COMPONENT_IMPL(VerticalLayout);

    VerticalLayout::VerticalLayout(const StringAtom& name)
        : Layout(componentType, name)
    {
        if (name.isEmpty())
        {
            setComponentName("VerticalLayout"_atom);
        }
        setFlex(Flex::FlexWidth);
        setVerticalAlign(Align::Top);
        setHorizontalAlign(Align::Center);
    }

    float VerticalLayout::getWidth() const
    {
        if (_width)
        {
            return *_width;
        }

        if (hasParent())
        {
            return getParent()->unsafeCastTo<Widget>()->getWidth();
        }
        return ImGui::GetContentRegionAvail().x;
    }

    float VerticalLayout::getHeight() const
    {
        if (_height)
        {
            return *_height + (_paddings.z + _paddings.w);
        }

        if (!hasParent() && (static_cast<int>(_flex) & static_cast<int>(Flex::FlexHeight)) != 0)
        {
            return ImGui::GetContentRegionAvail().y;
        }

        const float defaultGap = style().ItemSpacing.y;

        float height = 0;
        for (auto&& child : _children)
        {
            if (!child->isEnabled())
            {
                continue;
            }
            height += child->unsafeCastTo<Widget>()->getHeight();
            height += _spacing.value_or(defaultGap);
        }

        if (height != 0)
        {
            return height - _spacing.value_or(defaultGap) + (_paddings.z + _paddings.w);
        }

        return 0;
    }

    void VerticalLayout::setSpacing(float value)
    {
        _spacing = value;
    }

    void VerticalLayout::resetSpacing()
    {
        _spacing.reset();
    }

    float VerticalLayout::getSpacing() const
    {
        return _spacing.value_or(style().ItemSpacing.y);
    }

    void VerticalLayout::onAddChild(BaseComponent* newChild)
    {
        Widget::onAddChild(newChild);

        newChild->unsafeCastTo<Widget>()->setIsAutoDraw(false);
    }

    void VerticalLayout::recalcFlexChildren()
    {
        recalcFlexWidthChildren();
        recalcFlexHeightChildren();
    }

    void VerticalLayout::onTick(float delta)
    {
        Widget::onTick(delta);
    }

    void VerticalLayout::onDraw()
    {
        recalcFlexChildren();

        const auto originalCursor = ImGui::GetCursorPos();

        calcXOffsets();

        if (_secondAlign == Align::Top)
        {
            prepareAlignTop();
        }
        else if (_secondAlign == Align::Bottom)
        {
            prepareAlignBottom();
        }
        else if (_secondAlign == Align::SpaceBetween)
        {
            prepareAlignSpaceBetween();
        }
        else if (_secondAlign == Align::Center)
        {
            prepareAlignCenter();
        }
        else
        {
            Assert(false);
            prepareAlignTop();
        }

        directDraw();

        if (_width)
        {
            ImGui::SetCursorPosX(originalCursor.x + *_width);
        }
        ImGui::SetCursorPosY(originalCursor.y + getHeight());

        ImGui::Dummy(glm::vec2(0, 0));
    }

    void VerticalLayout::onInitialize()
    {
        Layout::onInitialize();
    }

    void VerticalLayout::prepareAlignSpaceBetween()
    {
        // do nothing
    }

    void VerticalLayout::prepareAlignTop()
    {
        // do nothing
    }

    void VerticalLayout::prepareAlignBottom()
    {
        const float defaultSpacing = style().ItemSpacing.y;

        if (hasChildren())
        {
            float spacing = getHeight();
            for (const auto& child : _children)
            {
                if (!child->isEnabled())
                {
                    continue;
                }
                spacing -= child->unsafeCastTo<Widget>()->getHeight();
                spacing -= _spacing.value_or(defaultSpacing);
            }
            spacing += _spacing.value_or(defaultSpacing);
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + spacing);
        }
    }

    void VerticalLayout::prepareAlignCenter()
    {
        const float defaultSpacing = style().ItemSpacing.y;

        if (hasChildren())
        {
            float spacing = getHeight();
            for (const auto& child : _children)
            {
                if (!child->isEnabled())
                {
                    continue;
                }
                spacing -= child->unsafeCastTo<Widget>()->getHeight();
                spacing -= _spacing.value_or(defaultSpacing);
            }
            spacing += _spacing.value_or(defaultSpacing);
            spacing /= 2.f;

            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + spacing);
        }
    }

    void VerticalLayout::directDraw()
    {
        const auto originalXCursor = ImGui::GetCursorPosX();

        float space = _spacing.value_or(0.f);
        if (_secondAlign == Align::SpaceBetween && hasChildren())
        {
            space = getHeight();
            for (auto&& child : _children)
            {
                if (!child->isEnabled())
                {
                    continue;
                }
                space -= child->unsafeCastTo<Widget>()->getHeight();
            }
            space /= _children.size() - 1ll;
        }

        if (_paddings.z != 0.f)
        {
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + _paddings.z);
        }

        std::size_t i = 0;

        const float defaultSpacing = style().ItemSpacing.y;
        for (auto&& child : _children)
        {
            if (!child->isEnabled())
            {
                continue;
            }
            ImGui::SetCursorPosX(originalXCursor + _xOffsets.at(i));

            child->unsafeCastTo<Widget>()->unhandledDraw();
            if (space != 0.f)
            {
                ImGui::SetCursorPosY(ImGui::GetCursorPosY() + space - defaultSpacing);
            }

            ++i;
        }

        if (_paddings.w != 0.f)
        {
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + _paddings.w);
        }
    }

    void VerticalLayout::calcXOffsets()
    {
        const auto ownWidth = getWidth();

        _xOffsets.resize(getChildrenCount());
        std::size_t i = 0;
        for (auto&& child : _children)
        {
            if (!child->isEnabled())
            {
                continue;
            }
            _xOffsets.at(i) = 0;
            const auto w = child->unsafeCastTo<Widget>()->getWidth();
            if (w != -1.f)
            {
                if (_align == Align::Right)
                {
                    _xOffsets.at(i) = ownWidth - w;
                }
                else if (_align == Align::Center)
                {
                    _xOffsets.at(i) = (ownWidth - w) / 2.f;
                }
            }
            ++i;
        }

        Assert(_xOffsets.size() == _children.size());
    }

    void VerticalLayout::recalcFlexWidthChildren()
    {
        const auto ownWidth = getWidth() - (_paddings.x + _paddings.y);
        for (auto&& child : _children)
        {
            if (!child->isEnabled())
            {
                continue;
            }
            const auto w = child->unsafeCastTo<Widget>();
            if ((static_cast<int>(w->getFlex()) & static_cast<int>(Flex::FlexWidth)) != 0)
            {
                w->setWidth(ownWidth);
            }
        }
    }

    void VerticalLayout::recalcFlexHeightChildren()
    {
        if (!atLeastOne(Flex::FlexHeight))
        {
            return;
        }

        const float defaultSpacing = style().ItemSpacing.y;
        float height = getHeight() - (_paddings.y + _paddings.y);

        int fixedCount = 0;
        int flexCount = 0;
        for (auto&& child : _children)
        {
            if (!child->isEnabled())
            {
                continue;
            }
            const auto w = child->unsafeCastTo<Widget>();
            const auto type = static_cast<int>(w->getFlex());
            if ((type & static_cast<int>(Flex::FlexHeight)) != 0)
            {
                ++flexCount;
            }
            else
            {
                height -= w->getHeight();
                height -= defaultSpacing;
                ++fixedCount;
            }
        }
        if (fixedCount != 0)
        {
            height += defaultSpacing;
        }

        int deCounter = flexCount;
        for (auto& child : _children)
        {
            if (!child->isEnabled())
            {
                continue;
            }
            auto w = child->unsafeCastTo<Widget>();
            if ((static_cast<int>(w->getFlex()) & static_cast<int>(Flex::FlexHeight)) != 0)
            {
                const float gap = deCounter != 0 ? defaultSpacing : 0;
                const float finalHeight
                    = std::max(0.f, height / static_cast<float>(flexCount) - gap);
                w->setHeight(finalHeight);
                --deCounter;
            }
            if (deCounter == 0)
            {
                break;
            }
        }
    }
} // namespace Core::Gui