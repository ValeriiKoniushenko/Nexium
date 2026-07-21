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
        class ListModelBased;
        template<class T>
        class LabelRow;

        class TextInput;

        class Button;
    } // namespace Gui

    CLASS();
    class ModalECSSearchPopUpEWC : public BaseEWC
    {
        ECS_DECL(ModalECSSearchPopUpEWC, Core::BaseEWC);

    public:
        void open(StringAtom text, const std::function<void(BaseComponent::Ptr)>& callback);
        static void Open(StringAtom text, const std::function<void(BaseComponent::Ptr)>& callback);

    protected:
        void onInitialize() override;

        void onDraw() override;

        void preOpenedEndWindowDraw() override;

        [[nodiscard]] bool beginWindowDraw() override;

        void endWindowDraw() override;

    protected:
        DelegateSubscriberPoolGuard _subscriptionPool;
        std::function<void(BaseComponent::Ptr)> _callback;

        StringAtom _caption = "ModalECSSearchPopUpEWC";
        Gui::VerticalLayout _layout;
        Gui::ListModelBased* _listView = nullptr;
        Gui::LabelRow<Gui::TextInput>* _nameInput = nullptr;
        Gui::Button* _okButton = nullptr;
        Gui::Button* _cancelButton = nullptr;

        bool _hasOpenRequest = false;
    };
} // namespace Core

#include "ModalECSSearchPopUp.generated.h" // added by the code generator. Better don't move it.
