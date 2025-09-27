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

#pragma once

#include "Core/Color.h"
#include "GameplaySystem/ECS/BaseComponent.h"
#include "GameplaySystem/ECS/Transformable.h"
#include "ImGui/imgui.h"

namespace Core::Gui
{
    // clang-format off
    CreateEnum(Align, int,
        None,
        Left,           // For horizontal align
        Right,          // For horizontal align
        SpaceBetween,   // For horizontal align
        Center,         // For horizontal & vertical align
        Top,            // For vertical align
        Bottom          // For vertical align
    );

    CreateEnum(Flex, int,
        Fixed,
        FlexWidth
    );
    // clang-format on

    class Widget : public BaseComponent
    {
        ECS_COMPONENT_DECL(Widget, BaseComponent);

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
        [[nodiscard]] virtual float getWidth() const = 0;

        [[nodiscard]] virtual float getHeight() const = 0;

        virtual void setWidth(float newWidth) = 0;

        virtual void setHeight(float newHeight) = 0;

        [[nodiscard]] bool isAutoDraw() const noexcept { return _autoDraw; }
        void setIsAutoDraw(bool value) noexcept { _autoDraw = value; }

        void setIsDrawOutline(bool value) noexcept { _isDrawOutline = value; }
        [[nodiscard]] bool getIsDrawOutline() const noexcept { return _isDrawOutline; }

        [[nodiscard]] Bounds getBounds() const noexcept;

        [[nodiscard]] Bounds getGlobalBounds() const noexcept;

        [[nodiscard]] Flex getFlex() const noexcept { return _flex; }
        void setFlex(Flex flex) noexcept { _flex = flex; }

        void disableWidget(bool value) noexcept { _isDisabledWidget = value; }
        [[nodiscard]] bool isDisabledWidget() const noexcept { return _isDisabledWidget; }

        /**
         * @brief Don't use this function to draw your component.
         * That's for internal usage.
         * If you want to draw your component - call the function BaseComponent::tick
         */
        void unhandledDraw();

    protected:
        virtual void onDraw() = 0;

        bool addChildValidator(BaseComponent* newChild) override;

        void onTick(float delta) override;

        void onInitialize() override;

        void drawOutline();

        virtual void preDraw();

        virtual void postDraw();

        [[nodiscard]] ImGuiStyle& style() const { return ImGui::GetStyle(); }

    protected:
        inline static int idGen = 0;
        int _id = 0; // internal id for ImGui

        glm::vec2 _pos = {};
        Flex _flex = Flex::Fixed;
        bool _autoDraw = true;
        bool _isDrawOutline = false;
        bool _isDisabledWidget = false;
    };
} // namespace Core::Gui

namespace ImGui
{
    bool OptPushStyleColor(ImGuiCol idx, const std::optional<Core::Color4>& col);

    bool OptPushStyleVar(ImGuiStyleVar idx, const std::optional<float>& col);
} // namespace ImGui