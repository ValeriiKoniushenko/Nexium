// Nexium
// Copyright 2018-2027 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "EditorInterleavedGraphicsDataAdapter.h"

#include "Editor/GuiComponents/Array.h"
#include "Editor/GuiComponents/LabelRow.h"
#include "Editor/GuiComponents/Misc.h"
#include "NxSubsystems/Graphics/GraphicsComponents.h"

using namespace Core::Gui;

namespace Core
{

    ECS_IMPL(ECSEditorInterleavedGraphicsDataAdapter);

    bool ECSEditorInterleavedGraphicsDataAdapter::canWorkWith(BaseComponent* component) const
    {
        return dynamic_cast<InterleavedGraphicsData*>(component) != nullptr;
    }

    void ECSEditorInterleavedGraphicsDataAdapter::onApplyAssetData(const nlohmann::json& json)
    {
        auto* comp = dynamic_cast<InterleavedGraphicsData*>(getTargetComponent());
        if (!Verify(comp)) [[unlikely]]
        {
            warnLog("Can't cast component to InterleavedGraphicsData, but it must be cast!");
            return;
        }

        _modifiers->input->clearData(true);
        for (auto&& modifier : comp->getDrawModifiers())
        {
            _modifiers->input->add(modifier, true);
        }
    }

    void ECSEditorInterleavedGraphicsDataAdapter::onInitialize()
    {
        ECSEditorMimeAdapter::onInitialize();

        constexpr float labelWidth = 120.0f;

        _layout.setPaddings(glm::vec4{ ImGui::GetStyle().ItemSpacing.x });

        _modifiers
            = _layout.addChildComponent<LabelRow<GraphicsModifiersArray>>("Modifiers", labelWidth);
        _modifiers->setVerticalAlign(Align::Top);
        _subscriptionPool << _modifiers->input->onChange->subscribeAndGetID([this]()
                                                                            { makeParentDirty(); });
        _subscriptionPool << _modifiers->input->onSave->subscribeAndGetID(
            [this](auto&& params)
            {
                auto* comp = dynamic_cast<InterleavedGraphicsData*>(getTargetComponent());
                if (!Verify(comp)) [[unlikely]]
                {
                    warnLog(
                        "Can't cast component to InterleavedGraphicsData, but it must be cast!");
                    return;
                }
                comp->setDrawModifiers(params);
            });
    }

    void ECSEditorInterleavedGraphicsDataAdapter::onDraw(float dt)
    {
        if (Gui::CollapsingHeader("InterleavedGraphicsData properties",
                                  ImGuiTreeNodeFlags_DefaultOpen))
        {
            _layout.tick(dt);
        }
    }
} // namespace Core
