// MIT License
//
// Copyright (c) 2019-2025 Valerii Koniushenko
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "ObjectSelectorManager.h"

#include "Graphics/Primitives/StaticMeshBundle.h"

namespace Core
{

    void ObjectSelectorManager::selectObject(BaseComponent* comp)
    {
        deselectAllAndClear();

        addSelectedObject(comp);
    }

    void ObjectSelectorManager::selectObject(StaticMeshBundle& bundle)
    {
        deselectAllAndClear();

        addSelectedObject(bundle);
    }

    void ObjectSelectorManager::selectObject(StaticMesh& mesh)
    {
        deselectAllAndClear();

        addSelectedObject(mesh);
    }

    void ObjectSelectorManager::deselectAllAndClear()
    {
        for (auto&& mesh : _selectedObjects)
        {
            mesh->setIsDrawOutline(false);
        }

        _selectedObjects.clear();
    }

    void ObjectSelectorManager::addSelectedObject(BaseComponent* comp)
    {
        if (!comp)
        {
            return;
        }

        if (auto* bundle = comp->tryCastTo<StaticMeshBundle>())
        {
            addSelectedObject(*bundle);
        }
        else if (auto* mesh = comp->tryCastTo<StaticMesh>())
        {
            addSelectedObject(*mesh);
        }
        else
        {
            // Assert("Was passed invalid component");
        }
    }

    void ObjectSelectorManager::addSelectedObject(StaticMeshBundle& bundle)
    {
        for (auto& mesh : bundle.getRenderTargets())
        {
            addSelectedObject(mesh);
        }
    }

    void ObjectSelectorManager::addSelectedObject(StaticMesh& mesh)
    {
        mesh.setIsDrawOutline(true);
        _selectedObjects.push_back(&mesh);
    }

} // namespace Core