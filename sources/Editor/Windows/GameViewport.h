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
#include "Editor/SceneGizmo.h"

namespace NX
{
    CLASS();
    class GameViewportEWC : public BaseFloatEWC
    {
        ECS_DECL(GameViewportEWC, NX::BaseFloatEWC);

    public:
        [[nodiscard]] const char* getIcon() override;
        [[nodiscard]] bool blocksPicking() const noexcept { return _blocksPicking; }

    protected:
        void onInitialize() override;
        void onUpdate() override;
        void onDraw() override;

    private:
        SceneGizmo _gizmo;
        bool _blocksPicking = false;
    };
} // namespace NX

#include "GameViewport.generated.h" // added by the code generator. Better don't move it.
