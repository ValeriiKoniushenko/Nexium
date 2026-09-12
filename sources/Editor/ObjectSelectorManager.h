// Nexium
// Copyright 2018-2027 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "Core/Delegate.h"
#include "NxWorld/Entities/Mesh/StaticMesh.h"

#include <unordered_map>

namespace Core
{
    class StaticMeshBundle;
    class StaticMesh;

    /// Use this class to select objects on the scene.
    /// @details mediator class to involve all needed functionality
    /// after picking up some scene's object.
    class ObjectSelectorManager
    {
    public:
        void toggleObject(BaseComponent* comp);
        void selectSingleObject(BaseComponent* comp);
        void addSelectedObject(BaseComponent* comp);

        void deselectSingleObject(BaseComponent* comp);
        void deselectAllAndClear();

        [[nodiscard]] bool isSelected(BaseComponent* comp) const;

        [[nodiscard]] std::unordered_map<void*, BaseComponent::Ptr>& getSelectedObjects()
        {
            return _selectedObjects;
        }

        /// @param BaseComponent* affected component
        /// @param bool selected is true; deselected is false
        Delegate<void(BaseComponent*, bool)>::Ptr onChange
            = Delegate<void(BaseComponent*, bool)>::Create();

    private:
        void changeSelectionState(BaseComponent* comp, bool newState);

    private:
        std::unordered_map<void*, BaseComponent::Ptr> _selectedObjects;
    };
} // namespace Core
