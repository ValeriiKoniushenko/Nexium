// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "BaseWindow.h"
#include "Editor/GuiComponents/VerticalLayout.h"

namespace NX
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
    class ModalECSSearchPopUpEWC : public BaseModalPopUp
    {
        ECS_DECL(ModalECSSearchPopUpEWC, NX::BaseModalPopUp);

    public:
        void open(StringAtom text, const std::function<void(BaseComponent::Ptr)>& callback);
        static void Open(StringAtom text, const std::function<void(BaseComponent::Ptr)>& callback);

    protected:
        void onInitialize() override;
        void onDraw() override;

        void onClose() override;

        void okButtonClicked();
        void cancelButtonClicked();

    protected:
        std::function<void(BaseComponent::Ptr)> _callback;

        StringAtom _caption = "ModalECSSearchPopUpEWC";
        Gui::VerticalLayout _layout;
        Gui::ListModelBased* _list = nullptr;
        Gui::LabelRow<Gui::TextInput>* _nameInput = nullptr;
        Gui::Button* _okButton = nullptr;
        Gui::Button* _cancelButton = nullptr;

        bool _hasOpenRequest = false;
        bool _wasManuallyEdited = false;
    };
} // namespace NX

#include "ModalECSSearchPopUp.generated.h" // added by the code generator. Better don't move it.
