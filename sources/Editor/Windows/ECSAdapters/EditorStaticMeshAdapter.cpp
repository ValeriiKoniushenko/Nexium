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

#include "EditorStaticMeshAdapter.h"

#include "Graphics/Primitives/StaticMesh.h"

using namespace Core::Gui;

namespace Core
{

    R_FRIEND_IMPL(ECSEditorStaticMeshAdapter);
    ECS_COMPONENT_IMPL(ECSEditorStaticMeshAdapter);

    StringAtom ECSEditorStaticMeshAdapter::getProcessedAssetType() const
    {
        return StaticMesh::componentType;
    }

    bool ECSEditorStaticMeshAdapter::canWorkWith(BaseComponent* component) const
    {
        return dynamic_cast<StaticMesh*>(component) != nullptr;
    }

    void ECSEditorStaticMeshAdapter::onApplyAssetData(const nlohmann::json&)
    {
        auto* comp = getTargetComponent()->tryCastTo<StaticMesh>();
        if (!Verify(comp)) [[unlikely]]
        {
            warnLog("Can't cast component to Actor, but it must be cast!");
            return;
        }
    }

    void ECSEditorStaticMeshAdapter::onInitialize()
    {
        ECSEditorMimeAdapter::onInitialize();

        constexpr float labelWidth = 120.0f;
    }

    void ECSEditorStaticMeshAdapter::onDraw(float dt)
    {
        // if (Gui::CollapsingHeader("StaticMesh properties", ImGuiTreeNodeFlags_DefaultOpen))
        // {
        //     _layout.tick(dt);
        // }
    }
} // namespace Core