// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "Widget.h"

#include "Foundation/Configs.h"
#include "InputDevices/Keyboard.h"
#include "InputDevices/Mouse.h"
#include "NxFundamental/ResourceManagement/JsonAdapter.h"

namespace Core::Gui
{
    ECS_IMPL(Widget);

    void Widget::unhandledDraw()
    {
        if (!isEnabled())
        {
            return;
        }

        _pos = ImGui::GetCursorPos();

#if defined(NEXIUM_DEBUG)
        if (Keyboard::IsKeyPressed(Widget::Input::editorImGuiShowRect)
            && getGlobalBounds().isIntersects(Mouse::GetPosition()))
        {
            drawOutline();
        }
#endif
        if (_isDrawOutline)
        {
            drawOutline();
        }

        ImGui::PushID(_id);
        preDraw();
        onDraw();
        postDraw();
        ImGui::PopID();
    }

    Widget::Bounds Widget::getBounds() const noexcept
    {
        return { .topLeft = glm::vec2(0), .bottomRight = glm::vec2(getWidth(), getHeight()) };
    }

    Widget::Bounds Widget::getGlobalBounds() const noexcept
    {
        const auto scrollX = ImGui::GetScrollX();
        const auto scrollY = ImGui::GetScrollY();

        auto out = getBounds() + _pos + ImGui::GetWindowPos(); // + _pos + ;
        out.topLeft.x -= scrollX;
        out.bottomRight.x -= scrollX;

        out.topLeft.y -= scrollY;
        out.bottomRight.y -= scrollY;

        return out;
    }

    bool Widget::addChildValidator(BaseComponent* newChild)
    {
        return !!newChild->tryCastTo<Widget>();
    }

    void Widget::onTick(float delta)
    {
        BaseComponent::onTick(delta);

        if (_autoDraw)
        {
            unhandledDraw();
        }
    }

    void Widget::onInitialize()
    {
        BaseComponent::onInitialize();
        _id = idGen++;
    }

    void Widget::drawOutline()
    {
        ImDrawList* draw_list = ImGui::GetWindowDrawList();

        constexpr float thickness = 2.f;
        auto [topLeft, bottomRight] = getGlobalBounds();
        bottomRight += thickness;
        topLeft -= thickness / 2.f;

        draw_list->AddRect(topLeft, bottomRight, IM_COL32(255, 255, 0, 255), // yellow color
                           0.0f,                                             // rounding
                           0,                                                // flags
                           thickness                                         // thickness
        );
    }

    void Widget::preDraw()
    {
        if (_isDisabledWidget)
        {
            ImGui::BeginDisabled(true);
        }
    }

    void Widget::postDraw()
    {
        if (_isDisabledWidget)
        {
            ImGui::EndDisabled();
        }
    }

    float Widget::Bounds::getWidth() const noexcept
    {
        return static_cast<float>(fabs(bottomRight.x - topLeft.x));
    }

    float Widget::Bounds::getHeight() const noexcept
    {
        return static_cast<float>(fabs(bottomRight.y - topLeft.y));
    }

    bool Widget::Bounds::isIntersects(glm::vec2 pos)
    {
        return pos.x >= topLeft.x && pos.x <= bottomRight.x && pos.y >= topLeft.y
               && pos.y <= bottomRight.y;
    }

    Widget::Bounds Widget::Bounds::operator+(glm::vec2 offset)
    {
        Bounds bounds = *this;
        bounds.topLeft += offset;
        bounds.bottomRight += offset;
        return bounds;
    }
    Widget::Bounds& Widget::Bounds::operator+=(glm::vec2 offset)
    {
        topLeft += offset;
        bottomRight += offset;
        return *this;
    }

} // namespace Core::Gui

namespace ImGui
{
    bool OptPushStyleColor(ImGuiCol idx, const std::optional<Core::Color4>& col)
    {
        if (col.has_value())
        {
            PushStyleColor(idx, Core::NormColor4::From(*col));
            return true;
        }
        return false;
    }

    bool OptPushStyleVar(ImGuiStyleVar idx, const std::optional<float>& col)
    {
        if (col.has_value())
        {
            PushStyleVar(idx, *col);
            return true;
        }
        return false;
    }
} // namespace ImGui
