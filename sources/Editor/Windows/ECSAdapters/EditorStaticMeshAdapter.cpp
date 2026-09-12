// Nexium
// Copyright 2018-2027 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "EditorStaticMeshAdapter.h"

#include "NxWorld/Entities/Mesh/StaticMesh.h"

using namespace Core::Gui;

namespace Core
{

    ECS_IMPL(ECSEditorStaticMeshAdapter);

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
