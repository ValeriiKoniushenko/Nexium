// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "Combo.h"

#include "ImGui/imgui_internal.h"
#include "ImGui/misc/cpp/imgui_stdlib.h"

namespace Core::Gui
{
    ECS_IMPL(BaseCombo);
    ECS_IMPL(ComboModelBased);

    ECS_IMPL(ComboView);

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

    StringAtom ComboModelBased::tryGetCurrentDataAsString() const
    {
        StringAtom item;
        (void)_dataProvider(_currentIndex, item);
        return item;
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
