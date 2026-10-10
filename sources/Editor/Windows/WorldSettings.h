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
#include "Editor/GuiComponents/Input.h"
#include "Editor/GuiComponents/Label.h"
#include "Editor/GuiComponents/LabelRow.h"
#include "Editor/GuiComponents/VerticalLayout.h"

#include <JustReflectMe/Number.h>
#include <type_traits>

namespace NX
{
    class GuiGenerator : public Gui::VerticalLayout
    {
    public:
        template<Utils::IsArithmetic T>
        void spawn(const Core::StringAtom& label, T& value, const RClassField& field)
        {
            using ValueType = std::conditional_t<std::is_integral_v<T>, int64_t, double>;
            using GuiValType = std::conditional_t<std::is_integral_v<ValueType>, int, float>;

            auto* c = addChildComponent<Gui::LabelRow<Gui::NumInput<GuiValType>>>();
            c->label->setText(label);
            c->label->setWidth(100.f);
            c->input->setInputtedData(value);
            if (field.attributes.min)
            {
                const auto v = std::get<ValueType>(*field.attributes.min);
                c->input->setMin(static_cast<ValueType>(v));
            }
            if (field.attributes.max)
            {
                const auto v = std::get<ValueType>(*field.attributes.max);
                c->input->setMax(static_cast<ValueType>(v));
            }
            if (field.attributes.step)
            {
                const auto v = std::get<int64_t>(*field.attributes.step);
                c->input->setStep(static_cast<ValueType>(v));
            }
            _subscriptionPool << c->input->onInput->subscribeAndGetID([&value](ValueType in)
                                                                      { value = in; });
        }

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
