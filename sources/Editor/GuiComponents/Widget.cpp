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

#include "Widget.h"

namespace Core
{

    ECS_REGISTER_NEW_COMPONENT_TYPE(Widget)

    void Widget::draw()
    {
        _pos = ImGui::GetCursorPos();
        if (_isDrawOutline)
        {
            drawOutline();
        }

        onDraw();
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
            draw();
        }
    }

    void Widget::drawOutline()
    {
        ImDrawList* draw_list = ImGui::GetWindowDrawList();

        // Calculate rect corners in screen space
        const auto size = ImVec2(getWidth(), getHeight());
        const auto wndPos = ImGui::GetWindowPos();
        const ImVec2 p_min = ImVec2(wndPos.x + _pos.x, wndPos.y + _pos.y);
        const ImVec2 p_max = ImVec2(p_min.x + size.x, p_min.y + size.y);

        // Draw yellow rect outline
        draw_list->AddRect(p_min, p_max, IM_COL32(255, 255, 0, 255), // yellow color
                           1.0f,                                     // rounding
                           0,                                        // flags
                           1.0f                                      // thickness
        );
    }

    ImVec4 colorToImVec4(const Color4& _color)
    {
        const auto color = NormColor4::From(_color);
        return ImVec4(color.r, color.g, color.b, color.a);
    }

} // namespace Core

namespace ImGui
{

    void PushStyleColor(ImGuiCol idx, const Core::Color4& col)
    {
        ImGui::PushStyleColor(idx, Core::colorToImVec4(col));
    }

    bool OptPushStyleColor(ImGuiCol idx, const std::optional<Core::Color4>& col)
    {
        if (col.has_value())
        {
            PushStyleColor(idx, *col);
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
