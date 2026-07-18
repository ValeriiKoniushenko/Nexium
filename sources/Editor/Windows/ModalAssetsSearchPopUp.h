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

#include "AssetsManager/ECSAsset.h"
#include "BaseWindow.h"
#include "Editor/GuiComponents/Input.h"
#include "Editor/GuiComponents/LabelRow.h"
#include "Editor/GuiComponents/VerticalLayout.h"

namespace Core
{

    namespace Gui
    {
        class ListModelBased;
        class Button;

        template<class T>
        class LabelRow;

        class TextInput;

    } // namespace Gui

    CLASS();
    class ModalAssetsSearchPopUpEWC : public BaseEWC
    {
        R_FRIEND_DECL(ModalAssetsSearchPopUpEWC, Core::BaseEWC);
        ECS_COMPONENT_DECL(ModalAssetsSearchPopUpEWC, BaseEWC);

    public:
        void open(StringAtom text);
        static void Open(StringAtom text);

    protected:
        void onInitialize() override;
        void onDraw() override;
        void preOpenedEndWindowDraw() override;
        [[nodiscard]] bool beginWindowDraw() override;
        void endWindowDraw() override;

        void okButtonClicked();
        void cancelButtonClicked();
        void onClose() override;
        void onOpen() override;
        DelegateSubscriberPoolGuard _subscriptionPool;

        StringAtom _caption = "ModalAssetsSearchPopUpEWC";

        Gui::LabelRow<Gui::TextInput>* _nameField = nullptr;
        Gui::VerticalLayout _layout;
        Gui::ListModelBased* _list = nullptr;
        Gui::Button* _okButton = nullptr;
        Gui::Button* _cancelButton = nullptr;

        bool _hasOpenRequest = false;
        bool _wasManuallyEdited = false;
    };
} // namespace Core

#include "ModalAssetsSearchPopUp.generated.h" // added by the code generator. Better don't move it.
