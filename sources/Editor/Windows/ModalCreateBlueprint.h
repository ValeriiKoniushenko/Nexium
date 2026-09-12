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

    CLASS();
    class ModalCreateBlueprintEWC : public BaseEWC
    {
        ECS_DECL(ModalCreateBlueprintEWC, Core::BaseEWC);

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

#include "ModalCreateBlueprint.generated.h" // added by the code generator. Better don't move it.
