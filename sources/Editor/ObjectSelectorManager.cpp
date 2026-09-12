// Nexium
// Copyright 2018-2027 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "ObjectSelectorManager.h"

namespace Core
{
    void ObjectSelectorManager::toggleObject(BaseComponent* comp)
    {
        if (isSelected(comp))
        {
            deselectSingleObject(comp);
        }
        else
        {
            addSelectedObject(comp);
        }
    }

    void ObjectSelectorManager::selectSingleObject(BaseComponent* comp)
    {
        deselectAllAndClear();
        addSelectedObject(comp);
    }

    void ObjectSelectorManager::deselectAllAndClear()
    {
        for (auto& obj : _selectedObjects | std::views::values)
        {
            if (obj) [[likely]]
            {
                changeSelectionState(obj.get(), false);
            }
        }

        _selectedObjects.clear();
    }

    void ObjectSelectorManager::addSelectedObject(BaseComponent* comp)
    {
        if (!comp) [[unlikely]]
        {
            return;
        }

        changeSelectionState(comp, true);
        _selectedObjects.emplace(comp, comp);
    }

    void ObjectSelectorManager::deselectSingleObject(BaseComponent* comp)
    {
        if (!comp) [[unlikely]]
        {
            return;
        }

        if (const auto it = _selectedObjects.find(comp); it != _selectedObjects.end())
        {
            changeSelectionState(comp, false);
            _selectedObjects.erase(it);
        }
    }

    bool ObjectSelectorManager::isSelected(BaseComponent* comp) const
    {
        return _selectedObjects.contains(comp);
    }

    void ObjectSelectorManager::changeSelectionState(BaseComponent* comp, bool newState)
    {
        if (auto* outliner = dynamic_cast<IOutliner*>(comp))
        {
            outliner->setIsDrawOutline(newState);
        }

        onChange->trigger(comp, newState);
    }

} // namespace Core
