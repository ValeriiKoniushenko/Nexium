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

#include "CheckBox.h"

namespace Core::Gui
{
    R_FRIEND_IMPL(CheckBox);
    ECS_COMPONENT_IMPL(CheckBox);

    glm::vec2 CheckBox::getRealSize() const
    {
        return _size;
    }

    void CheckBox::setWidth(float newWidth)
    {
        Assert(false, "CheckBox doesn't support any resizing");
        _size.x = newWidth;
    }

    void CheckBox::setHeight(float newHeight)
    {
        Assert(false, "CheckBox doesn't support any resizing");
        _size.y = newHeight;
    }

    void CheckBox::onDraw()
    {
        ImGui::PushItemWidth(_size.x);

        const bool origValue = _currentValue;
        ImGui::Checkbox("", &_currentValue);
        if (origValue != _currentValue)
        {
            onChange->trigger(_currentValue);
        }

        ImGui::PopItemWidth();
    }

    void CheckBox::onInitialize()
    {
        Widget::onInitialize();
        if (_name.isEmpty())
        {
            setComponentName("CheckBox"_atom);
        }

        _size = glm::vec2(ImGui::GetFrameHeight());
    }
} // namespace Core::Gui