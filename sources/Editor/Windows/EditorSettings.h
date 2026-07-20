/*
 * MIT License
 *
 * Copyright (c) 2018-2027 Valerii Koniushenko
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

#include "BaseWindow.h"
#include "Editor/GuiComponents/HorizontalLayout.h"
#include "Editor/GuiComponents/VerticalLayout.h"

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
            R_FRIEND_DECL(BaseListItem, Core::Gui::HorizontalLayout);
            ECS_COMPONENT_DECL(BaseListItem, HorizontalLayout);

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
            R_FRIEND_DECL(KeymapItem, Core::Internal::BaseListItem);
            ECS_COMPONENT_DECL(KeymapItem, BaseListItem);

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
            R_FRIEND_DECL(ColorItem, Core::Internal::BaseListItem);
            ECS_COMPONENT_DECL(ColorItem, BaseListItem);

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
        R_FRIEND_DECL(EditorSettingsEWC, Core::BaseFloatEWC);
        ECS_COMPONENT_DECL(EditorSettingsEWC, BaseFloatEWC);

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
        Gui::VerticalLayout _layouts[Menu_COUNT];
        Menu _currentMenu = static_cast<Menu>(0);
        int _defaultTreeNodeFlags = ImGuiTreeNodeFlags_SpanAvailWidth;
    };
} // namespace Core

#include "EditorSettings.generated.h" // added by the code generator. Better don't move it.
