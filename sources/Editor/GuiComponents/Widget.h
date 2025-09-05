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

#pragma once

#include "Core/Color.h"
#include "GameplaySystem/ECS/BaseComponent.h"
#include "GameplaySystem/ECS/Transformable.h"
#include "ImGui/imgui.h"

namespace Core
{

    class Widget : public BaseComponent
    {
        ECS_REGISTER_NEW_COMPONENT(Widget, BaseComponent);

    public:
        struct Bounds
        {
            glm::vec2 topLeft = {};
            glm::vec2 bottomRight = {};

            [[nodiscard]] float getWidth() const noexcept
            {
                return fabs(bottomRight.x - topLeft.x);
            }
            [[nodiscard]] float getHeight() const noexcept
            {
                return fabs(bottomRight.y - topLeft.y);
            }

            [[nodiscard]] bool isIntersects(glm::vec2 pos)
            {
                return pos.x >= topLeft.x && pos.x <= bottomRight.x && pos.y >= topLeft.y
                       && pos.y <= bottomRight.y;
            }

            [[nodiscard]] Bounds operator+(glm::vec2 offset)
            {
                Bounds bounds = *this;
                bounds.topLeft += offset;
                bounds.bottomRight += offset;
                return bounds;
            }

            [[nodiscard]] Bounds& operator+=(glm::vec2 offset)
            {
                topLeft += offset;
                bottomRight += offset;
                return *this;
            }
        };

    public:
        Widget(Widget&& other) noexcept;
        Widget& operator=(Widget&& other) noexcept;
        Widget(const Widget& other);
        Widget& operator=(const Widget& other);

        [[nodiscard]] virtual float getWidth() const = 0;
        [[nodiscard]] virtual float getHeight() const = 0;

        [[nodiscard]] bool isAutoDraw() const noexcept { return _autoDraw; }
        void setIsAutoDraw(bool value) noexcept { _autoDraw = value; }
        virtual void onDraw() = 0;
        void draw();

        void setIsDrawOutline(bool value) noexcept { _isDrawOutline = value; }
        [[nodiscard]] bool getIsDrawOutline() const noexcept { return _isDrawOutline; }

        [[nodiscard]] Bounds getBounds() const noexcept;
        [[nodiscard]] Bounds getGlobalBounds() const noexcept;

    protected:
        bool addChildValidator(BaseComponent* newChild) override;
        void onTick(float delta) override;
        void drawOutline();
        [[nodiscard]] ImGuiStyle& style() const { return ImGui::GetStyle(); }

    protected:
        inline static int idGen = 0;
        int id = idGen++; // internal id for ImGui

        glm::vec2 _pos = {};
        bool _autoDraw = true;
        bool _isDrawOutline = false;
    };

} // namespace Core

namespace ImGui
{
    bool OptPushStyleColor(ImGuiCol idx, const std::optional<Core::Color4>& col);
    bool OptPushStyleVar(ImGuiStyleVar idx, const std::optional<float>& col);
} // namespace ImGui