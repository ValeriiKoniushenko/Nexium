// Nexium
// Copyright 2018-2027 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "../NxECSBasedEditor.h"
#include "Editor/GuiComponents/VerticalLayout.h"
#include "Editor/Windows/ObjectPropertiesWindow.h"

namespace Core
{
    CLASS();
    class ECSEditorInterleavedGraphicsDataAdapter : public ECSEditorMimeAdapter
    {
        ECS_DECL(ECSEditorInterleavedGraphicsDataAdapter, Core::ECSEditorMimeAdapter);

    public:
        [[nodiscard]] bool canWorkWith(BaseComponent* component) const override;
        [[nodiscard]] StringAtom getProcessedAssetType() const override
        {
            return "InterleavedGraphicsData"_atom;
        }

    protected:
        void onApplyAssetData(const nlohmann::json& json) override;
        void onInitialize() override;
        void onDraw(float dt) override;

    protected:
        Gui::VerticalLayout _layout;

        Gui::LabelRow<GraphicsModifiersArray>* _modifiers = nullptr;

        DelegateSubscriberPoolGuard _subscriptionPool;
    };

} // namespace Core

#include "EditorInterleavedGraphicsDataAdapter.generated.h" // added by the code generator. Better don't move it.
