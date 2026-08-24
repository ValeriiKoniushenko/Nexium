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

#include "ObjectSelectorManager.h"

#include "GameplaySystem/Framework/GameInstance.h"

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