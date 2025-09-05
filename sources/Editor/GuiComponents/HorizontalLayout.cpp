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

#include "HorizontalLayout.h"

#include "ImGui/imgui_internal.h"

namespace Core
{
    ECS_REGISTER_NEW_COMPONENT_TYPE(HorizontalLayout)

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

        if (hasParent())
        {
            return getParent()->unsafeCastTo<Widget>()->getWidth();
        }
        return ImGui::GetContentRegionAvail().x;
    }

    float HorizontalLayout::getHeight() const
    {
        if (_height)
        {
            return *_height;
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
        return maxHeight;
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
        auto* window = ImGui::GetCurrentWindow();
        window->DC.CurrLineTextBaseOffset = 0.0f;

        const auto originalCursor = ImGui::GetCursorPos();

        calcYOffsets();

        if (_align.cast() == Align::Left)
        {
            drawAlignLeft();
        }
        else if (_align.cast() == Align::Right)
        {
            drawAlignRight();
        }
        else if (_align.cast() == Align::SpaceBetween)
        {
            drawAlignSpaceBetween();
        }
        else if (_align.cast() == Align::Center)
        {
            drawAlignCenter();
        }
        else
        {
            Assert(false);
            drawAlignLeft();
        }

        if (_width)
        {
            ImGui::SetCursorPosX(originalCursor.x + *_width);
        }
        ImGui::SetCursorPosY(originalCursor.y);

        ImGui::Dummy(glm::vec2(0, 0));
    }

    void HorizontalLayout::onInitialize()
    {
        Widget::onInitialize();
        if (_name.isEmpty())
        {
            setComponentName("HorizontalLayout");
        }

        if (getVerticalAlign().cast() == Align::None)
        {
            setVerticalAlign(Align::Center);
        }
        if (getHorizontalAlign().cast() == Align::None)
        {
            setHorizontalAlign(Align::Left);
        }
    }

    void HorizontalLayout::drawAlignSpaceBetween()
    {
        float spacing = 0.f;
        if (hasChildren())
        {
            spacing = getWidth();
            const float defaultSpacing = style().ItemSpacing.x;
            for (const auto& child : _children)
            {
                spacing -= child->unsafeCastTo<Widget>()->getWidth();
                spacing -= defaultSpacing;
            }
            spacing += defaultSpacing;
            spacing /= _children.size() - 1ll;
        }

        const auto originalYCursor = ImGui::GetCursorPosY();

        std::size_t i = 0;
        for (auto&& child : _children)
        {
            ImGui::SetCursorPosY(originalYCursor + _yOffsets.at(i++));
            child->unsafeCastTo<Widget>()->draw();
            ImGui::SameLine();
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + spacing);
        }
    }

    void HorizontalLayout::drawAlignLeft()
    {
        const auto originalYCursor = ImGui::GetCursorPosY();
        std::size_t i = 0;
        for (auto&& child : _children)
        {
            ImGui::SetCursorPosY(originalYCursor + _yOffsets.at(i++));
            child->unsafeCastTo<Widget>()->draw();
            ImGui::SameLine();
        }
    }

    void HorizontalLayout::drawAlignRight()
    {
        if (hasChildren())
        {
            float spacing = getWidth();
            for (const auto& child : _children)
            {
                spacing -= child->unsafeCastTo<Widget>()->getWidth();
                spacing -= style().ItemSpacing.x;
            }

            ImGui::Dummy(glm::vec2(spacing, 0));
            ImGui::SameLine();
        }

        const auto originalYCursor = ImGui::GetCursorPosY();
        std::size_t i = 0;

        for (auto&& child : _children)
        {
            ImGui::SetCursorPosY(originalYCursor + _yOffsets.at(i++));
            child->unsafeCastTo<Widget>()->draw();
            ImGui::SameLine();
        }
    }

    void HorizontalLayout::drawAlignCenter()
    {
        if (hasChildren())
        {
            float spacing = getWidth();
            for (const auto& child : _children)
            {
                spacing -= child->unsafeCastTo<Widget>()->getWidth();
                spacing -= style().ItemSpacing.x;
            }
            spacing -= style().ItemSpacing.x;
            spacing /= 2.f;

            ImGui::Dummy(glm::vec2(spacing, 0));
            ImGui::SameLine();
        }

        const auto originalYCursor = ImGui::GetCursorPosY();
        std::size_t i = 0;

        for (auto&& child : _children)
        {
            ImGui::SetCursorPosY(originalYCursor + _yOffsets.at(i++));
            child->unsafeCastTo<Widget>()->draw();
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

        Assert(_yOffsets.size() == _children.size());
    }

} // namespace Core