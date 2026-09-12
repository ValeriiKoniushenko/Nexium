// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "List.h"

namespace Core::Gui
{
    ECS_IMPL(BaseList);
    ECS_IMPL(ListView);

    ECS_IMPL(ListModelBased);

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
                if (!_filter.isEmpty() && !_items.at(i).regexFind(_filter))
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
                if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
                {
                    onDoubleClickSelect->trigger(_items.at(i));
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

                if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
                {
                    onDoubleClickSelect->trigger(_cache.at(i).first, string);
                }
            }
            ImGui::EndListBox();
        }
    }
} // namespace Core::Gui
