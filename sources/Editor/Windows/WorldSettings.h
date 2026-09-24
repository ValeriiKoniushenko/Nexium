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
#include "Editor/GuiComponents/VerticalLayout.h"

namespace NX
{
    CLASS();
    class WorldSettingsEWC : public BaseFloatEWC
    {
        ECS_DECL(WorldSettingsEWC, NX::BaseFloatEWC);

    public:
        [[nodiscard]] const char* getIcon() override;

    protected:
        void onInitialize() override;
        void onOpen() override;
        void onUpdate() override;

        void onDraw() override;

    protected:
        // Global
        Gui::VerticalLayout _globalLayout;
        Gui::TextInput* _cameraInputField = nullptr;
        Gui::Button* _changeCameraButton = nullptr;
        Gui::Button* _showCameraButton = nullptr;
        Gui::Button* _resetCameraButton = nullptr;

        // Lightning
        Gui::VerticalLayout _lightningLayout;
        Gui::Color3Input* _color3Input = nullptr;
        Gui::FloatInput* _ambientStrength = nullptr;
        Gui::FloatInput* _minLightStrength = nullptr;
        Gui::FloatInput* _specularStrength = nullptr;
        Gui::FloatInput* _specularPow = nullptr;
        Gui::Float3Input* _sunDirection = nullptr;
    };
} // namespace NX

#include "WorldSettings.generated.h"
