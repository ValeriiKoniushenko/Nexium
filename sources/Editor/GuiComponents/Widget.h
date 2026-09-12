// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "Core/Color.h"
#include "ImGui/imgui.h"
#include "InputDevices/Keyboard.h"
#include "NxFundamental/ECS/BaseComponent.h"
#include "NxFundamental/ResourceManagement/JsonAdapter.h"

namespace Core::Gui
{
    ENUM_CLASS();
    enum class Align
    {
        None,
        Left,         // For horizontal align
        Right,        // For horizontal align
        SpaceBetween, // For horizontal align
        Center,       // For horizontal & vertical align
        Top,          // For vertical align
        Bottom        // For vertical align
    };

    ENUM_CLASS();
    enum class Flex
    {
        None = 0,
        Fixed = 1,
        FlexWidth = 1 << 1,
        FlexHeight = 1 << 2,                        // Alpha version, poor supporting
        FlexWidthAndHeight = FlexWidth | FlexHeight // Alpha version, poor supporting
    };

    CLASS();
    class Widget : public BaseComponent
    {
        ECS_DECL(Widget, Core::BaseComponent);

    public:
        struct Input
        {
            constexpr static Keyboard::Key editorImGuiShowRect = Keyboard::Key::F2;
        };

        struct Bounds
        {
            glm::vec2 topLeft = {};
            glm::vec2 bottomRight = {};

            [[nodiscard]] float getWidth() const noexcept;

            [[nodiscard]] float getHeight() const noexcept;

            [[nodiscard]] bool isIntersects(glm::vec2 pos);

            [[nodiscard]] Bounds operator+(glm::vec2 offset);

            [[nodiscard]] Bounds& operator+=(glm::vec2 offset);
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

        /// Don't use this function to draw your component.
        /// That's for internal usage.
        /// If you want to draw your component - call the function BaseComponent::tick
        void unhandledDraw();

    protected:
        void onTick(float delta) override;
        virtual void onDraw() = 0;

        bool addChildValidator(BaseComponent* newChild) override;

        void onInitialize() override;

        void drawOutline();

        virtual void preDraw();

        virtual void postDraw();

        [[nodiscard]] ImGuiStyle& style() const { return ImGui::GetStyle(); }

    protected:
        inline static int idGen = 0;
        int _id = 0; // internal id for ImGui

        FIELD();
        glm::vec2 _pos = glm::vec2{ 0.f, 0.f };
        FIELD();
        Core::Gui::Flex _flex = Core::Gui::Flex::Fixed;
        FIELD();
        bool _autoDraw = true;
        FIELD();
        bool _isDrawOutline = false;
        FIELD();
        bool _isDisabledWidget = false;
    };

    template<class T>
    concept IsWidget = std::derived_from<std::remove_reference_t<T>, Widget>;

} // namespace Core::Gui

namespace ImGui
{
    bool OptPushStyleColor(ImGuiCol idx, const std::optional<Core::Color4>& col);

    bool OptPushStyleVar(ImGuiStyleVar idx, const std::optional<float>& col);
} // namespace ImGui

#include "Widget.generated.h" // added by the code generator. Better don't move it.
