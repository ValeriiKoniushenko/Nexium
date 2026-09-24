// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

// Module: Nexium::Editor

#pragma once

#include "BaseWindow.h"
#include "Editor/ForwardDeclarations.h"
#include "Editor/GuiComponents/VerticalLayout.h"
#include "NxFundamental/Assets/ECSAsset.h"

namespace NX
{
    CLASS();
    class ModalAssetsSearchPopUpEWC : public BaseModalPopUp
    {
        ECS_DECL(ModalAssetsSearchPopUpEWC, NX::BaseModalPopUp);

    public:
        static void Open(StringAtom text);

    protected:
        void onDraw() override;
        void onInitialize() override;

        void okButtonClicked();
        void cancelButtonClicked();

        void onClose() override;

    protected:
        Gui::LabelRow<Gui::TextInput>* _nameField = nullptr;
        Gui::VerticalLayout _layout;
        Gui::ListModelBased* _list = nullptr;
        Gui::Button* _okButton = nullptr;
        Gui::Button* _cancelButton = nullptr;

        bool _hasOpenRequest = false;
        bool _wasManuallyEdited = false;
    };
} // namespace NX

#include "ModalAssetsSearchPopUp.generated.h" // added by the code generator. Better don't move it.
