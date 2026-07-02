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

#include "List.h"

namespace Core::Gui
{
    R_FRIEND_IMPL(BaseList);
    ECS_COMPONENT_IMPL(BaseList);

    R_FRIEND_IMPL(ListView);
    ECS_COMPONENT_IMPL(ListView);

    R_FRIEND_IMPL(ListModelBased);
    ECS_COMPONENT_IMPL(ListModelBased);

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

    void BaseList::resetListNavigation()
    {
        resetRegexFilter();
        resetCurrentIndex();
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
                if (!_filter.isEmpty() && !_items.at(i).regexMatch(_filter))
                {
                    continue;
                }

                if (_needsFocus && i == _currentIndex)
                {
                    ImGui::SetKeyboardFocusHere();
                    _needsFocus = false;
                }

                const bool isSelected = (_currentIndex == i);
                if (ImGui::Selectable(_items.at(i).c_str(), isSelected))
                {
                    _currentIndex = i;
                    onSelect->trigger(_items.at(i));
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
        const std::function<const void*(std::size_t, StringAtom&)>& callback)
    {
        _dataProvider = callback;
    }

    void ListModelBased::setSizeProvider(const std::function<std::size_t()>& callback)
    {
        _sizeProvider = callback;
    }

    void ListModelBased::setCurrentIndex(std::size_t i)
    {
        if (Verify(_sizeProvider && _dataProvider))
        {
            _currentIndex = std::min(i, _sizeProvider() - 1);
            StringAtom item;
            _currentData = _dataProvider(_currentIndex, item);
        }
    }

    StringAtom ListModelBased::tryGetCurrentDataAsString() const
    {
        const auto size = _sizeProvider();
        StringAtom item;
        (void)_dataProvider(_currentIndex, item);
        return item;
    }

    void ListModelBased::resetListNavigation()
    {
        BaseList::resetListNavigation();

        _currentData = nullptr;
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
            const void* ptr = _dataProvider(i, label);

            _cache[i].first = ptr;
            _cache[i].second = std::move(label);
        }

        if (ImGui::BeginListBox("", _size))
        {
            for (std::size_t i = 0; i < _cache.size(); ++i)
            {
                const auto& string = _cache.at(i).second;
                if (!_filter.isEmpty() && !string.regexFind(_filter))
                {
                    continue;
                }

                if (_needsFocus && i == _currentIndex)
                {
                    ImGui::SetKeyboardFocusHere();
                    _needsFocus = false;
                }

                const bool isSelected = (_currentIndex == i);
                if (ImGui::Selectable(string.c_str(), isSelected))
                {
                    _currentIndex = i;
                    _currentData = _cache.at(i).first;
                    onSelect->trigger(_cache.at(i).first, string);
                }
                if (isSelected)
                {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndListBox();
        }
    }
} // namespace Core::Gui