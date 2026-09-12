// Nexium
// Copyright 2018-2027 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "EditorTransformableAdapter.h"

#include "ECS/Transformable.h"
#include "Editor/GuiComponents/Misc.h"

using namespace Core::Gui;

namespace Core
{

    ECS_IMPL(ECSEditorTransformableAdapter);

    bool ECSEditorTransformableAdapter::canWorkWith(BaseComponent* component) const
    {
        return dynamic_cast<Transformable*>(component) != nullptr;
    }

    void ECSEditorTransformableAdapter::onApplyAssetData(const nlohmann::json&)
    {
        auto* comp = dynamic_cast<Transformable*>(getTargetComponent());
        if (!Verify(comp)) [[unlikely]]
        {
            warnLog("Can't cast component to Actor, but it must be cast!");
            return;
        }
    }

    void ECSEditorTransformableAdapter::onInitialize()
    {
        ECSEditorMimeAdapter::onInitialize();

        constexpr float labelWidth = 120.0f;
    }

    void ECSEditorTransformableAdapter::onDraw(float dt)
    {
        if (Gui::CollapsingHeader("Transformable properties", ImGuiTreeNodeFlags_DefaultOpen))
        {
            _layout.tick(dt);
        }
    }
} // namespace Core