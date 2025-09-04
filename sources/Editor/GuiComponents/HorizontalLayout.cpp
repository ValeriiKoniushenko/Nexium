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

namespace Core
{

    float HorizontalLayout::getWidth()
    {
        if (hasParent())
        {
            return getParent()->unsafeCastTo<Widget>()->getWidth();
        }
        return ImGui::GetContentRegionAvail().x;
    }

    float HorizontalLayout::getHeight()
    {
        if (hasParent())
        {
            return getParent()->unsafeCastTo<Widget>()->getHeight();
        }
        return ImGui::GetContentRegionAvail().y;
    }
    void HorizontalLayout::onAddChild(BaseComponent* newChild)
    {
        Widget::onAddChild(newChild);
        newChild->unsafeCastTo<Widget>()->setIsAutoDraw(false);
    }

    void HorizontalLayout::onTick(float delta)
    {
        Widget::onTick(delta);
        int pushed = 0;

        float spacing = getWidth();
        if (hasChildren())
        {
            for (auto& child : _children)
            {
                spacing -= child->unsafeCastTo<Widget>()->getWidth();
            }
            spacing /= _children.size() - 1;
        }

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(spacing, style().ItemSpacing.y));
        ++pushed;

        for (auto&& child : _children)
        {
            child->unsafeCastTo<Widget>()->onDraw();
            ImGui::SameLine();
        }

        ImGui::PopStyleVar(pushed);
    }

    void HorizontalLayout::onDraw()
    {
    }

    void HorizontalLayout::onInitialize()
    {
        Widget::onInitialize();
    }

} // namespace Core