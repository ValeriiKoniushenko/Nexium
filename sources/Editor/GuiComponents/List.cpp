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

#include "List.h"

#include "ImGui/imgui_internal.h"
#include "ImGui/misc/cpp/imgui_stdlib.h"
#include "Misc/ImGuiHelper.h"

namespace Core
{

    ECS_REGISTER_NEW_COMPONENT_TYPE(BaseList);
    ECS_REGISTER_NEW_COMPONENT_TYPE(ListView);
    ECS_REGISTER_NEW_COMPONENT_TYPE(ListModelBased);

    glm::vec2 BaseList::getRealSize() const
    {
        return _size;
    }

    void BaseList::setWidth(float newWidth)
    {
        _size.x = newWidth;
    }

    void BaseList::setHeight(float newHeight)
    {
        _size.y = newHeight;
    }

    void BaseList::onDraw()
    {
    }

    void BaseList::onInitialize()
    {
        Widget::onInitialize();
        if (_name.isEmpty())
        {
            setComponentName("List"_atom);
        }
    }

    void ListView::onDraw()
    {
        if (ImGui::BeginListBox("", _size))
        {
            for (std::size_t i = 0; i < _items.size(); ++i)
            {
                const bool isSelected = (_currentItem == i);
                if (ImGui::Selectable(_items.at(i).c_str(), isSelected))
                {
                    _currentItem = i;
                    onSelect.trigger(this, _items.at(i));
                }
                if (isSelected)
                {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndListBox();
        }
    }

    void ListModelBased::setDataProvider(
        const std::function<void*(std::size_t, StringAtom&)>& callback)
    {
        _dataProvider = callback;
    }

    void ListModelBased::setSizeProvider(const std::function<std::size_t()>& callback)
    {
        _sizeProvider = callback;
    }

    void ListModelBased::setCurrentIndex(std::size_t i) noexcept
    {
        if (Verify(_sizeProvider && _dataProvider))
        {
            _currentIndex = std::min(i, _sizeProvider() - 1);
            StringAtom item;
            _currentData = _dataProvider(_currentIndex, item);
        }
    }

    void ListModelBased::onDraw()
    {
        if (!_dataProvider || !_sizeProvider)
        {
            return;
        }

        _cache.resize(_sizeProvider());
        for (std::size_t i = 0; i < _cache.size(); ++i)
        {
            StringAtom label;
            void* ptr = _dataProvider(i, label);

            _cache[i].first = ptr;
            _cache[i].second = std::move(label);
        }

        if (ImGui::BeginListBox("", _size))
        {
            for (std::size_t i = 0; i < _cache.size(); ++i)
            {
                const bool isSelected = (_currentIndex == i);
                if (ImGui::Selectable(_cache.at(i).second.c_str(), isSelected))
                {
                    _currentIndex = i;
                    _currentData = _cache.at(i).first;
                    onSelect.trigger(this, _cache.at(i).first);
                }
                if (isSelected)
                {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndListBox();
        }
    }

} // namespace Core