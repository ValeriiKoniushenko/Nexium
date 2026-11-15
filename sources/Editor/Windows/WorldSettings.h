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

#include "BaseWindow.h"
#include "Editor/ForwardDeclarations.h"
#include "Editor/GuiComponents/PropertyGrid.h"

namespace NX
{
    /// The world's settings, generated from the FIELD()s of `World` (section "Global", with the
    /// current camera as a custom row) and of its `LightningProps` (section "Lightning").
    CLASS();
    class WorldSettingsEWC : public BaseFloatEWC
    {
        ECS_DECL_NO_CNSTR(WorldSettingsEWC, NX::BaseFloatEWC);

    public:
        explicit WorldSettingsEWC(const StringAtom& name = ""_atom);

        [[nodiscard]] const char* getIcon() override;

    protected:
        void onInitialize() override;
        void onOpen() override;
        void onClose() override;
        void onDraw() override;

    protected:
        Gui::PropertyGrid::Ptr _worldGrid;
        Gui::PropertyGrid::Ptr _lightningGrid;
    };
} // namespace NX

#include "WorldSettings.generated.h"
