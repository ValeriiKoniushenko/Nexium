// Nexium
// Copyright 2018-2027 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "BaseWindow.h"
#include "Core/Timer.h"

namespace Core
{
    CLASS();
    class EditorMenuBarWindowEWC : public BaseMenuBarEWC
    {
        ECS_DECL(EditorMenuBarWindowEWC, Core::BaseMenuBarEWC);

    public:
        EditorMenuBarWindowEWC(const EditorMenuBarWindowEWC&) = delete;
        EditorMenuBarWindowEWC(EditorMenuBarWindowEWC&&) noexcept = delete;
        EditorMenuBarWindowEWC& operator=(const EditorMenuBarWindowEWC&) = delete;
        EditorMenuBarWindowEWC& operator=(EditorMenuBarWindowEWC&&) noexcept = delete;
        ~EditorMenuBarWindowEWC() override = default;

    protected:
        void onInitialize() override;

        void onDraw() override;

        void onUpdate() override;

    private:
        inline static auto _fpsText = "FPS: ";
        inline static auto _fpsTextTemplate = "FPS: 99999";
        StringAtom _cachedFpsText = _fpsText;
        Repeater _slowUpdater;
        float _fpsTextSize = 50.f;
        float _simulationButton = 50.f;
    };
} // namespace Core

#include "EditorMenuBarWindow.generated.h" // added by the code generator. Better don't move it.
