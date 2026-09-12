// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "CheckBox.h"

namespace Core::Gui
{
    ECS_IMPL(CheckBox);

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