// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "Spacer.h"

namespace Core::Gui
{
    ECS_IMPL(Spacer);

    glm::vec2 Spacer::getRealSize() const
    {
        return _size + style().ItemSpacing;
    }

    void Spacer::setWidth(float newWidth)
    {
        _size.x = newWidth;
    }

    void Spacer::setHeight(float newHeight)
    {
        _size.y = newHeight;
    }

    void Spacer::scaleCurrentWidth(float mlt)
    {
        _size.x *= mlt;
    }

    void Spacer::scaleCurrentHeight(float mlt)
    {
        _size.y *= mlt;
    }

    void Spacer::onDraw()
    {
        ImGui::Dummy(_size);
    }

    void Spacer::onInitialize()
    {
        Widget::onInitialize();
        if (_name.isEmpty())
        {
            setComponentName("Spacer"_atom);
        }
    }
} // namespace Core::Gui