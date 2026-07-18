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

#include "Combo.h"

#include "ImGui/imgui_internal.h"
#include "ImGui/misc/cpp/imgui_stdlib.h"

namespace Core::Gui
{
    R_FRIEND_IMPL(BaseCombo);
    ECS_COMPONENT_IMPL(BaseCombo);

    R_FRIEND_IMPL(ComboModelBased);
    ECS_COMPONENT_IMPL(ComboModelBased);

    R_FRIEND_IMPL(ComboView);
    ECS_COMPONENT_IMPL(ComboView);

    glm::vec2 BaseCombo::getRealSize() const
    {
        return _size;
    }

    void BaseCombo::setWidth(float newWidth)
    {
        _size.x = newWidth;
    }

    void BaseCombo::setHeight(float newHeight)
    {
        _size.y = newHeight;
    }

    void BaseCombo::onInitialize()
    {
        Widget::onInitialize();
        if (_name.isEmpty())
        {
            setComponentName("View"_atom);
        }

        if (_size.y == 0.f)
        {
            _size.y = ImGui::GetFontSize() + style().FramePadding.y * 2.0f;
        }
    }

    void ComboView::onDraw()
    {
        ImGui::PushItemWidth(_size.x);
        StringAtom preview = _currentItem < _items.size() ? _items.at(_currentItem) : ""_atom;

        if (ImGui::BeginCombo("", preview.c_str()))
        {
            for (std::size_t i = 0; i < _items.size(); ++i)
            {
                const bool isSelected = (_currentItem == i);
                if (ImGui::Selectable(_items.at(i).c_str(), isSelected))
                {
                    _currentItem = i;
                    onSelect->trigger(_items.at(i));
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

    void ComboModelBased::setDataProvider(

        const std::function<const void*(std::size_t, StringAtom&)>& callback)
    {
        _dataProvider = callback;
    }

    void ComboModelBased::setSizeProvider(const std::function<std::size_t()>& callback)
    {
        _sizeProvider = callback;
    }

    void ComboModelBased::setCurrentIndex(std::size_t i) noexcept
    {
        if (Verify(_sizeProvider && _dataProvider))
        {
            _currentIndex = std::min(i, _sizeProvider() - 1);
            StringAtom item;
            _currentData = _dataProvider(_currentIndex, item);
        }
    }

    void ComboModelBased::onDraw()
    {
        if (!_dataProvider || !_sizeProvider)
        {
            return;
        }

        ImGui::PushItemWidth(_size.x);

        _cache.resize(_sizeProvider());
        for (std::size_t i = 0; i < _cache.size(); ++i)
        {
            StringAtom label;
            const void* ptr = _dataProvider(i, label);

            _cache[i].first = ptr;
            _cache[i].second = std::move(label);
        }

        const auto preview
            = _currentIndex < _cache.size() ? _cache.at(_currentIndex).second.c_str() : "";

        if (ImGui::BeginCombo("", preview))
        {
            for (std::size_t i = 0; i < _cache.size(); ++i)
            {
                const bool isSelected = (_currentIndex == i);
                if (ImGui::Selectable(_cache.at(i).second.c_str(), isSelected))
                {
                    _currentIndex = i;
                    _currentData = _cache.at(i).first;
                    onSelect->trigger(_cache.at(i).first);
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
} // namespace Core::Gui