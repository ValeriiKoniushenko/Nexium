// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "BaseWindow.h"
#include "Editor/GuiComponents/VerticalLayout.h"

namespace Core
{
    namespace Gui
    {
        class Button;
        class TextInput;

        template<Utils::IsArithmetic T>
        class NumInput;

        using IntInput = NumInput<int>;
        using FloatInput = NumInput<float>;

        class Color3Input;

        template<std::size_t Size, Utils::IsArithmetic Type>
        class VecNumInput;

        using Float3Input = VecNumInput<3, float>;
    } // namespace Gui

    CLASS();
    class WorldSettingsEWC : public BaseFloatEWC
    {
        ECS_DECL(WorldSettingsEWC, Core::BaseFloatEWC);

    public:
        [[nodiscard]] const char* getIcon() override;

    protected:
        void onInitialize() override;
        void onOpen() override;
        void onUpdate() override;

        void onDraw() override;

    protected:
        DelegateSubscriberPoolGuard _subscriptionPool;
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
} // namespace Core

#include "WorldSettings.generated.h"
