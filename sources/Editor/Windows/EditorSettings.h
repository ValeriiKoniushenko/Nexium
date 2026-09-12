// Nexium
// Copyright 2018-2027 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "BaseWindow.h"
#include "Editor/GuiComponents/HorizontalLayout.h"
#include "Editor/GuiComponents/VerticalLayout.h"

#include <array>

namespace Core
{
    namespace Gui
    {
        class Label;
        class TextInput;
        class Button;
    } // namespace Gui

    namespace Internal
    {

        CLASS();
        class BaseListItem : public Gui::HorizontalLayout
        {
            ECS_DECL(BaseListItem, Core::Gui::HorizontalLayout);

        public:
            [[nodiscard]] virtual bool containsString(const StringAtom& str);

            virtual void setReadOnly(bool value = true) = 0;

            void setLabel(const StringAtom& label);

        protected:
            void onInitialize() override;

        protected:
            Gui::Label* _label = nullptr;
        };

        CLASS();
        class KeymapItem : public BaseListItem
        {
            ECS_DECL(KeymapItem, Core::Internal::BaseListItem);

        public:
            void setButtonName(const StringAtom& label);

            void setReadOnly(bool value = true) override;

            [[nodiscard]] bool containsString(const StringAtom& str) override;

        protected:
            void onInitialize() override;

        protected:
            Gui::Button* _button = nullptr;
            Gui::Button* _resetButton = nullptr;
        };

        CLASS();
        class ColorItem : public BaseListItem
        {
            ECS_DECL(ColorItem, Core::Internal::BaseListItem);

        public:
            void setReadOnly(bool value = true) override;

            void setInputData(const StringAtom& data);

        protected:
            void onInitialize() override;

        protected:
            Gui::TextInput* _colorInput = nullptr;
        };

    } // namespace Internal

    CLASS();
    class EditorSettingsEWC : public BaseFloatEWC
    {
        ECS_DECL(EditorSettingsEWC, Core::BaseFloatEWC);

    public:
        [[nodiscard]] const char* getIcon() override;

    protected:
        enum Menu
        {
            Menu_Appearance,
            Menu_Keymap,
            Menu_COUNT
        };

    protected:
        void onOpen() override;

        void onDraw() override;

        void drawSettingsTree();

        void setupCommonLayoutSettings();

        void createPage_Keymap();

        void createPage_Appearance();

    protected:
        std::array<Gui::VerticalLayout, Menu_COUNT> _layouts;
        Menu _currentMenu = static_cast<Menu>(0);
        int _defaultTreeNodeFlags = ImGuiTreeNodeFlags_SpanAvailWidth;
    };
} // namespace Core

#include "EditorSettings.generated.h" // added by the code generator. Better don't move it.
