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
#include "Editor/GuiComponents/InspectorStyle.h"
#include "Editor/GuiComponents/PropertyGrid.h"
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
#include "Scenes/SceneTreeWindow.h"


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

        const float labelWidth = Gui::InspectorStyle::Default().wideLabelWidth;
        // ===================== GLOBAL =========================
        {
            auto* h = _globalLayout.addChildComponent<Gui::LabelRow<Gui::HorizontalLayout>>();
            h->label->setText("Camera");
            h->label->setWidth(labelWidth);

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
        Assert(!_lightningGrid);
        _lightningGrid = Gui::PropertyGrid::Create();
        _lightningGrid->setStableId("Lightning"_atom);
        _lightningGrid->setSections(Gui::PropertySections::Flat);
        _lightningGrid->setLabelWidth(labelWidth);
        _lightningGrid->setTarget(
            []
            {
                auto* world = GetWorld();
                return world ? Reflection::ObjectRef(world->lightning) : Reflection::ObjectRef{};
            });
    }

    void WorldSettingsEWC::onClose()
    {
        BaseEWC::onClose();

        _lightningGrid = nullptr;

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
            if (_lightningGrid)
            {
                _lightningGrid->tick(GetWorld()->getTimeDelta());
            }
        }
    }

} // namespace NX
