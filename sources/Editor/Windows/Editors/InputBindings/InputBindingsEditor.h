// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "BindingSettingsWidget.h"
#include "BindingsListWidget.h"
#include "Editor/Windows/BaseWindow.h"
#include "NxWorld/Framework/InputController.h"

#include <optional>

namespace NX
{
    class NxECSBasedEditorEWC;

    CLASS();
    class InputBindingsEditor : public BaseFloatEWC
    {
        ECS_DECL(InputBindingsEditor, NX::BaseFloatEWC);

    public:
        void setBindings(const std::vector<InputController::Binding>& bindings);
        void setTarget(InputController* controller, NxECSBasedEditorEWC* owner = nullptr);
        void selectBinding(std::size_t index);

    protected:
        void onInitialize() override;

        [[nodiscard]] bool beginWindowDraw() override;
        void onDraw() override;
        void onClose() override;

    private:
        void refreshBindingsList();
        void addBinding();
        void deleteSelectedBinding();
        void applyBindings();
        void validateTarget();

    private:
        BindingsListWidget _bindingsList;
        BindingSettingsWidget _bindingSettings;
        std::vector<InputController::Binding> _bindings;
        std::optional<std::size_t> _selectedBinding;
        std::size_t _bindingSession = 0;
        Core::WeakPtr<InputController> _targetController;
        Core::WeakPtr<BaseComponent> _owner;
        bool _hasTarget = false;
        bool _hasOwner = false;
        Core::DelegateSubscriber _selectionSubscription;
        Core::DelegateSubscriber _addBindingSubscription;
    };
} // namespace NX

#include "InputBindingsEditor.generated.h" // added by the code generator. Better don't move it.
