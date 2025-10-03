/*
 * MIT License
 *
 * Copyright (c) 2018-2025 Valerii Koniushenko
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

#include "Widget.h"

#include "../../Misc/Configs.h"
#include "InputDevices/Keyboard.h"
#include "InputDevices/Mouse.h"

namespace Core::Gui
{
    ECS_COMPONENT_IMPL(Widget)

    void Widget::unhandledDraw()
    {
        if (!isEnabled())
        {
            return;
        }

        _pos = ImGui::GetCursorPos();

#if defined(DEBUG)
        if (Keyboard::IsKeyPressed(Core::Config::Keyboard::editorImGuiShowRect)
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