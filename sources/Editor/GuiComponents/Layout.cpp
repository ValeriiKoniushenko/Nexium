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

#include "Layout.h"

namespace Core::Gui
{
    ECS_COMPONENT_IMPL(Layout);
    R_FRIEND_IMPL(Layout);

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