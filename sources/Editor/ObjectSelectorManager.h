/*
 * MIT License
 *
 * Copyright (c) 2018-2025 Valerii Koniushenko
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

#pragma once

#include "Core/Delegate.h"
#include "Graphics/Primitives/StaticMesh.h"

#include <unordered_map>

namespace Core
{
    class StaticMeshBundle;
    class StaticMesh;

    /**
     * Use this class to select objects on the scene.
     * @details mediator class to involve all needed functionality
     * after picking up some scene's object.
     */
    class ObjectSelectorManager
    {
    public:
        void selectObject(BaseComponent* comp);

        void addSelectedObject(BaseComponent* comp);

        void deselectAllAndClear();

        [[nodiscard]] bool isSelected(BaseComponent* comp) const;

        [[nodiscard]] std::unordered_map<void*, BaseComponent::Ptr>& getSelectedObjects()
        {
            return _selectedObjects;
        }

        /**
         * @param BaseComponent* affected component
         * @param bool selected is true; deselected is false
         */
        Delegate<void(BaseComponent*, bool)>::Ptr onChange
            = Delegate<void(BaseComponent*, bool)>::Create();

    private:
        void tryToSelectGeneralComponent(BaseComponent* comp);

        void tryToDeselectGeneralComponent();

    private:
        std::unordered_map<void*, BaseComponent::Ptr> _selectedObjects;
        BaseComponent* _generalSelectedComponent = nullptr;
    };
} // namespace Core