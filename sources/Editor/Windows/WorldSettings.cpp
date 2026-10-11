// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "WorldSettings.h"

#include "Core/String.h"
#include "Editor/EditorIntegration.h"
#include "Editor/ForwardDeclarations.h"
#include "Editor/GuiComponents/Button.h"
#include "Editor/GuiComponents/GuiGenerator.h"
#include "Editor/GuiComponents/HorizontalLayout.h"
#include "Editor/GuiComponents/Input.h"
#include "Editor/GuiComponents/Label.h"
#include "Editor/GuiComponents/LabelRow.h"
#include "Editor/GuiComponents/Misc.h"
#include "Editor/GuiComponents/VecInput.h"
#include "Editor/IconsFontAwesome.h"
#include "Editor/Windows/BaseWindow.h"
#include "NxWorld/Entities/Camera/Camera.h"
#include "NxWorld/Framework/GameInstanceAccess.h"
#include "NxWorld/Framework/World.h"
#include "SceneTreeWindow.h"

#include <JustReflectMe/Adapter.h>
#include <memory>

using namespace NX;

namespace NX
{
    WorldSettingsEWC::WorldSettingsEWC(const StringAtom& name)
        : BaseFloatEWC(componentType, name)
    {
        if (name.isEmpty())
        {
            setComponentName("World settings"_atom);
        }
    }

    ECS_IMPL(WorldSettingsEWC);

    const char* WorldSettingsEWC::getIcon()
    {
        return ICON_FA_SUN_O;
    }

    void WorldSettingsEWC::onInitialize()
    {
        BaseFloatEWC::onInitialize();
    }

    void WorldSettingsEWC::onOpen()
    {
        BaseFloatEWC::onOpen();

        constexpr float defaultWidth = 120.f;
        // ===================== GLOBAL =========================
        {
            auto* h = _globalLayout.addChildComponent<Gui::LabelRow<Gui::HorizontalLayout>>();
            h->label->setText("Camera");
            h->label->setWidth(defaultWidth);

            _cameraInputField = h->input->addChildComponent<Gui::TextInput>();
            _cameraInputField->setReadOnly(true);
            _cameraInputField->setFlex(Gui::Flex::FlexWidth);

            _changeCameraButton = h->input->addChildComponent<Gui::Button>(ICON_FA_SUN_O);
            _changeCameraButton->setWidth(30.f);
            _subscriptionPool << _changeCameraButton->onClick->subscribeAndGetID(
                []() { Editor::NotificationPopUp::Warning("Not implemented").show(); });

            _showCameraButton = h->input->addChildComponent<Gui::Button>(ICON_FA_EYE);
            _showCameraButton->setWidth(30.f);
            _subscriptionPool << _showCameraButton->onClick->subscribeAndGetID(
                []()
                {
                    if (!GetWorld()->currentCamera)
                    {
                        return;
                    }

                    if (auto sceneTree = GetEditor()->getWindow<SceneTreeWindowEWC>())
                    {
                        sceneTree->highlightSpecificObject(GetWorld()->currentCamera);
                    }
                });

            _resetCameraButton = h->input->addChildComponent<Gui::Button>(ICON_FA_UNDO);
            _resetCameraButton->setWidth(30.f);
            _subscriptionPool << _resetCameraButton->onClick->subscribeAndGetID([]()
                                                                                { ResetCamera(); });
        }

        // ===================== LIGHTNING =========================

        static const auto fields = R<decltype(GetWorld()->lightning)>::GetFieldsMap();

        Assert(!_lightningLayout);

        using LigtningGuiT = Gui::ObjectBasedGuiGenerator<decltype(GetWorld()->lightning)>;
        _lightningLayout = std::make_unique<LigtningGuiT>();

        if (auto* l = dynamic_cast<LigtningGuiT*>(_lightningLayout.get()))
        {
            l->setObjectGetter([]() { return &GetWorld()->lightning; });
            l->generateGui();
        }
        else
        {
            errorLogAndAssert(
                "Impossible to create Gui::ObjectBasedGuiGenerator<decltype(GetWorld()->lightning) "
                "to create GUI");
        }

        for (auto&& f : fields)
        {
            _lightningLayout->spawn(f.first.c_str(), GetWorld()->lightning.color, f.second);
        }
        // gg.spawn("color", GetWorld()->lightning.color, fields.at("color"));
        // gg.spawn("specularStrength", GetWorld()->lightning.specularStrength,
        //          fields.at("specularStrength"));
        // gg.spawn("minLightStrength", GetWorld()->lightning.minLightStrength,
        //          fields.at("minLightStrength"));
        // gg.spawn("ambientStrength", GetWorld()->lightning.ambientStrength,
        //          fields.at("ambientStrength"));
        // gg.spawn("specularPow", GetWorld()->lightning.specularPow, fields.at("specularPow"));
        //  gg.spawn("sunDirection", GetWorld()->lightning.sunDirection, fields.at("sunDirection"));
    }

    void WorldSettingsEWC::onClose()
    {
        BaseEWC::onClose();

        _lightningLayout->despawnEverything();

        _globalLayout.removeAllChildren();
        _cameraInputField = nullptr;
        _changeCameraButton = nullptr;
        _showCameraButton = nullptr;
        _resetCameraButton = nullptr;
    }

    void WorldSettingsEWC::onDraw()
    {
        if (_showCameraButton)
        {
            _showCameraButton->disableWidget(!GetWorld()->currentCamera);
        }
        if (_changeCameraButton)
        {
            _changeCameraButton->disableWidget(!GetWorld()->currentCamera);
        }
        if (_resetCameraButton)
        {
            _resetCameraButton->disableWidget(!GetWorld()->currentCamera);
        }
        if (_cameraInputField)
        {
            std::string str = "";
            if (GetWorld()->currentCamera)
            {
                str = GetWorld()->currentCamera->getComponentName().c_str();
            }
            _cameraInputField->setInputtedData(std::move(str));
        }

        if (Gui::CollapsingHeader("Global", ImGuiTreeNodeFlags_DefaultOpen))
        {
            _globalLayout.tick(GetWorld()->getTimeDelta());
        }

        if (Gui::CollapsingHeader("Lightning", ImGuiTreeNodeFlags_DefaultOpen))
        {
            _lightningLayout->tick(GetWorld()->getTimeDelta());
        }
    }

} // namespace NX
