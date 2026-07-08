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
#include "Editor/GuiComponents/VerticalLayout.h"

namespace Core
{

    namespace Gui
    {
        template<class T>
        class LabelRow;

        class ListModelBased;
        class Button;
        class TextInput;
        class Label;
    } // namespace Gui

    class ModalCreateBlueprintEWC : public BaseEWC
    {
        ECS_COMPONENT_DECL(ModalCreateBlueprintEWC, BaseEWC);

    public:
        void open(StringAtom text);
        static void Open(StringAtom text);

    protected:
        void onInitialize() override;

        void onDraw() override;

        void onClose() override;

        void resetGUI();

        void preOpenedEndWindowDraw() override;

        [[nodiscard]] bool beginWindowDraw() override;

        void endWindowDraw() override;

        void performBlueprintCreation(const std::string& type, const std::string& name,
                                      const std::string& path);

        void tryToDetectReplacingOfExistingFile();

        void okButtonClicked();
        void cancelButtonClicked();

        void onOpen() override;

    protected:
        DelegateSubscriberPoolGuard _subscriptionPool;

        StringAtom _caption = "ModalCreateBlueprintEWC";
        Gui::VerticalLayout _layout;
        Gui::ListModelBased* _list = nullptr;
        Gui::LabelRow<Gui::TextInput>* _nameField = nullptr;
        Gui::LabelRow<Gui::TextInput>* _typeField = nullptr;
        Gui::LabelRow<Gui::TextInput>* _pathField = nullptr;
        Gui::Label* _errorOutput = nullptr;
        Gui::Button* _okButton = nullptr;
        Gui::Button* _cancelButton = nullptr;
        bool _hasOpenRequest = false;
    };
} // namespace Core
