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

#include "BaseWindow.h"
#include "Editor/GuiComponents/Button.h"
#include "Editor/GuiComponents/HorizontalLayout.h"
#include "Editor/GuiComponents/VerticalLayout.h"
#include "Graphics/Texture.h"

namespace Core
{
    namespace Gui
    {
        class Label;
    }

    class EditorSettingsEWC : public BaseFloatEWC
    {
        ECS_COMPONENT_DECL(EditorSettingsEWC, BaseFloatEWC);

    public:
        class KeymapItem : public Gui::HorizontalLayout
        {
            ECS_COMPONENT_DECL(KeymapItem, HorizontalLayout);

        public:
            [[nodiscard]] bool containsString(const StringAtom& str);
            void setLabel(const StringAtom& label);
            void setButtonName(const StringAtom& label);
            void setReadOnly(bool value = true);

        protected:
            void onInitialize() override;

        protected:
            Gui::Label* _label = nullptr;
            Gui::Button* _button = nullptr;
            Gui::Button* _resetButton = nullptr;
        };

    public:
        [[nodiscard]] const char* getIcon() override { return ICON_FA_COG; }

    protected:
        enum Menu
        {
            Menu_Keymap,
            Menu_About,
            Menu_COUNT
        };

    protected:
        void onOpen() override;
        void onDraw() override;

        void drawSettingsTree();

        void setupCommonLayoutSettings();
        void createPage_Keymap();

    protected:
        Gui::VerticalLayout _layouts[Menu_COUNT];
        Menu _currentMenu = static_cast<Menu>(0);
    };

} // namespace Core
