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
#include "Editor/GuiComponents/HorizontalLayout.h"

namespace Core
{
    namespace Gui
    {
        class Button;
    }

    CLASS();
    class ModalPopUp : public BaseEWC
    {
        ECS_DECL_NO_CNSTR(ModalPopUp, Core::BaseEWC);

    public:
        explicit ModalPopUp(const Core::StringAtom& name = ""_atom);

        void open(StringAtom text, const std::function<void(bool)>& okOrCancelCallback);

        static void Open(StringAtom text, const std::function<void(bool)>& okOrCancelCallback);

    protected:
        void onInitialize() override;

        void onDraw() override;

        void preOpenedEndWindowDraw() override;

        [[nodiscard]] bool beginWindowDraw() override;

        void endWindowDraw() override;

    protected:
        DelegateSubscriberPoolGuard _subscriptionPool;

        Gui::HorizontalLayout _layout;
        Gui::Button* _okButton = nullptr;
        Gui::Button* _cancelButton = nullptr;

        std::function<void(bool)> _okOrCancelCallback;
        StringAtom _text;
        StringAtom _caption;

        bool _hasOpenRequest = false;
    };
} // namespace Core

#include "ModalPopUp.generated.h" // added by the code generator. Better don't move it.
