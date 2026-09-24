// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

// Module: Nexium::Editor

#pragma once

#include "../NxECSBasedEditor.h"
#include "Editor/GuiComponents/VerticalLayout.h"

namespace NX
{
    CLASS();
    class ECSEditorTransformableAdapter : public ECSEditorMimeAdapter
    {
        ECS_DECL(ECSEditorTransformableAdapter, NX::ECSEditorMimeAdapter);

    public:
        [[nodiscard]] bool canWorkWith(BaseComponent* component) const override;
        [[nodiscard]] StringAtom getProcessedAssetType() const override
        {
            return "Transformable"_atom;
        }

    protected:
        void onApplyAssetData(const nlohmann::json& json) override;
        void onInitialize() override;
        void onDraw(float dt) override;

    protected:
        Gui::VerticalLayout _layout;
    };

} // namespace NX

#include "EditorTransformableAdapter.generated.h" // added by the code generator. Better don't move it.
