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

#include "Combo.h"

#include "ImGui/imgui_internal.h"
#include "ImGui/misc/cpp/imgui_stdlib.h"
#include "Misc/ImGuiHelper.h"

namespace Core
{

    ECS_REGISTER_NEW_COMPONENT_TYPE(Combo);

    glm::vec2 Combo::getRealSize() const
    {
        return _size;
    }

    void Combo::setWidth(float newWidth)
    {
        _size.x = newWidth;
    }

    void Combo::setHeight(float newHeight)
    {
        _size.y = newHeight;
    }

    void Combo::onDraw()
    {
        ImGui::PushItemWidth(_size.x);
        StringAtom preview = _currentItem < _items.size() ? _items.at(_currentItem) : ""_atom;

        if (ImGui::BeginCombo(_name.c_str(), preview.c_str()))
        {
            for (std::size_t i = 0; i < _items.size(); ++i)
            {
                const bool isSelected = (_currentItem == i);
                if (ImGui::Selectable(_items.at(i).c_str(), isSelected))
                {
                    _currentItem = i;
                    onSelect.trigger(_items.at(i));
                }
                if (isSelected)
                {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }
        ImGui::PopItemWidth();
    }

    void Combo::onInitialize()
    {
        Widget::onInitialize();
        if (_name.isEmpty())
        {
            setComponentName("Combo"_atom);
        }

        if (_size.y == 0.f)
        {
            _size.y = ImGui::CalcTextSize(_name.c_str()).y + style().FramePadding.y * 2.0f;
        }
    }
} // namespace Core