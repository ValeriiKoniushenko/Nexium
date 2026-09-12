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
#include "Editor/GuiComponents/Input.h"
#include "Editor/GuiComponents/LabelRow.h"
#include "Editor/GuiComponents/VerticalLayout.h"
#include "NxFundamental/Assets/ECSAsset.h"

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
        ECS_DECL(ModalAssetsSearchPopUpEWC, Core::BaseEWC);

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

    protected:
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
