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

#include "Core/Delegate.h"
#include "NxFundamental/ECS/BaseComponent.h"

#include <unordered_map>

namespace NX
{
    /// Use this class to select objects on the scene.
    /// @details mediator class to involve all needed functionality
    /// after picking up some scene's object.
    class ObjectSelectorManager
    {
    public:
        void toggleObject(NX::BaseComponent* comp);
        void selectSingleObject(NX::BaseComponent* comp);
        void addSelectedObject(NX::BaseComponent* comp);

        void deselectSingleObject(NX::BaseComponent* comp);
        void deselectAllAndClear();

        [[nodiscard]] bool isSelected(NX::BaseComponent* comp) const;

        [[nodiscard]] std::unordered_map<void*, NX::BaseComponent::Ptr>& getSelectedObjects()
        {
            return _selectedObjects;
        }

        /// @param BaseComponent* affected component
        /// @param bool selected is true; deselected is false
        Delegate<void(NX::BaseComponent*, bool)>::Ptr onChange
            = Delegate<void(NX::BaseComponent*, bool)>::Create();

    private:
        void changeSelectionState(NX::BaseComponent* comp, bool newState);

    private:
        std::unordered_map<void*, NX::BaseComponent::Ptr> _selectedObjects;
    };
} // namespace NX
