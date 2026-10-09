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
#include "Core/Delegate.h"
#include "Editor/ForwardDeclarations.h"
#include "Editor/GuiComponents/VerticalLayout.h"

namespace NX
{
    class GuiGenerator : public Gui::VerticalLayout
    {
    public:
        void spawn(const Core::StringAtom& label, int& value);
        void spawn(const Core::StringAtom& label, float& value);

        void despawnEverything();

    protected:
        DelegateSubscriberPoolGuard _subscriptionPool;
    };

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

        GuiGenerator gg;
    };
} // namespace NX

#include "WorldSettings.generated.h"
