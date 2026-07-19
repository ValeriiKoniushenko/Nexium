/*
 * MIT License
 *
 * Copyright (c) 2018-2027 Valerii Koniushenko
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "WorldSettings.h"

#include "Editor/GuiComponents/Button.h"
#include "Editor/GuiComponents/HorizontalLayout.h"
#include "Editor/GuiComponents/Input.h"
#include "Editor/GuiComponents/Label.h"
#include "Editor/GuiComponents/LabelRow.h"
#include "Editor/GuiComponents/Misc.h"
#include "Editor/GuiComponents/VecInput.h"
#include "GameplaySystem/Framework/GameInstance.h"
#include "Misc/IconsFontAwesome.h"
#include "SceneTreeWindow.h"

namespace Core
{
    ECS_COMPONENT_IMPL(WorldSettingsEWC);
    R_FRIEND_IMPL(WorldSettingsEWC);

    const char* WorldSettingsEWC::getIcon()
    {
        return ICON_FA_SUN_O;
    }

    void WorldSettingsEWC::onInitialize()
    {
        BaseFloatEWC::onInitialize();

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
                    if (!gGameInstance->currentCamera)
                    {
                        return;
                    }

                    if (auto sceneTree = GetEditor().getWindow<SceneTreeWindowEWC>())
                    {
                        sceneTree->highlightSpecificObject(gGameInstance->currentCamera);
                    }
                });

            _resetCameraButton = h->input->addChildComponent<Gui::Button>(ICON_FA_UNDO);
            _resetCameraButton->setWidth(30.f);
            _subscriptionPool << _resetCameraButton->onClick->subscribeAndGetID(
                []() { gGameInstance->resetCamera(); });
        }

        // ===================== LIGHTNING =========================
        {
            auto* h = _lightningLayout.addChildComponent<Gui::HorizontalLayout>();
            h->setHorizontalAlign(Gui::Align::SpaceBetween);
            h->addChildComponent<Gui::Label>("Light color");
            _color3Input = h->addChildComponent<Gui::Color3Input>();
            _color3Input->setInputtedData(GetWorld().lightning.color);
            _subscriptionPool << _color3Input->onInput->subscribeAndGetID(
                [](Color3 color) { GetWorld().lightning.color = color.toNorm(); });
        }
        {
            auto* h = _lightningLayout.addChildComponent<Gui::HorizontalLayout>();
            h->setHorizontalAlign(Gui::Align::SpaceBetween);
            h->addChildComponent<Gui::Label>("Ambient strength");
            _ambientStrength = h->addChildComponent<Gui::FloatInput>();
            _ambientStrength->setInputtedData(GetWorld().lightning.ambientStrength);
            _ambientStrength->setMin(0.f);
            _ambientStrength->setMax(10.f);
            _ambientStrength->setStep(0.05f);

            _subscriptionPool << _ambientStrength->onInput->subscribeAndGetID(
                [](float value) { GetWorld().lightning.ambientStrength = value; });
        }
        {
            auto* h = _lightningLayout.addChildComponent<Gui::HorizontalLayout>();
            h->setHorizontalAlign(Gui::Align::SpaceBetween);
            h->addChildComponent<Gui::Label>("Min light strength");
            _minLightStrength = h->addChildComponent<Gui::FloatInput>();
            _minLightStrength->setInputtedData(GetWorld().lightning.minLightStrength);
            _minLightStrength->setMin(0.f);
            _minLightStrength->setMax(10.f);
            _minLightStrength->setStep(0.05f);
            _subscriptionPool << _minLightStrength->onInput->subscribeAndGetID(
                [](float value) { GetWorld().lightning.minLightStrength = value; });
        }
        {
            auto* h = _lightningLayout.addChildComponent<Gui::HorizontalLayout>();
            h->setHorizontalAlign(Gui::Align::SpaceBetween);
            h->addChildComponent<Gui::Label>("Specular strength");
            _specularStrength = h->addChildComponent<Gui::FloatInput>();
            _specularStrength->setMin(0.f);
            _specularStrength->setMax(10.f);
            _specularStrength->setStep(0.05f);

            _specularStrength->setInputtedData(GetWorld().lightning.specularStrength);
            _subscriptionPool << _specularStrength->onInput->subscribeAndGetID(
                [](float value) { GetWorld().lightning.specularStrength = value; });
        }
        {
            auto* h = _lightningLayout.addChildComponent<Gui::HorizontalLayout>();
            h->setHorizontalAlign(Gui::Align::SpaceBetween);
            h->addChildComponent<Gui::Label>("Specular pow");
            _specularPow = h->addChildComponent<Gui::FloatInput>();
            _specularPow->setMin(0.01f);
            _specularPow->setMax(4096.f);
            _specularPow->setStep(2.f);

            _specularPow->setInputtedData(GetWorld().lightning.specularPow);
            _subscriptionPool << _specularPow->onInput->subscribeAndGetID(
                [](float value) { GetWorld().lightning.specularPow = value; });
        }
        {
            auto* h = _lightningLayout.addChildComponent<Gui::HorizontalLayout>();
            h->setHorizontalAlign(Gui::Align::SpaceBetween);
            h->addChildComponent<Gui::Label>("Sun light direction");
            _sunDirection = h->addChildComponent<Gui::Float3Input>();
            _sunDirection->setFlex(Gui::Flex::Fixed);
            _sunDirection->setWidth(240.f);

            for (auto* i : _sunDirection->inputs)
            {
                i->setMin(-1.f);
                i->setMax(1.f);
                i->setStep(.01f);
            }

            _sunDirection->setInputtedData(GetWorld().lightning.sunDirection);
            _subscriptionPool << _sunDirection->onInput->subscribeAndGetID(
                [](glm::vec3 value) { GetWorld().lightning.sunDirection = value; });
        }
    }
    void WorldSettingsEWC::onOpen()
    {
        BaseFloatEWC::onOpen();

        if (Verify(_color3Input))
        {
            _color3Input->setInputtedData(GetWorld().lightning.color);
        }
        if (Verify(_ambientStrength))
        {
            _ambientStrength->setInputtedData(GetWorld().lightning.ambientStrength);
        }
        if (Verify(_minLightStrength))
        {
            _minLightStrength->setInputtedData(GetWorld().lightning.minLightStrength);
        }
        if (Verify(_specularStrength))
        {
            _specularStrength->setInputtedData(GetWorld().lightning.specularStrength);
        }
        if (Verify(_specularPow))
        {
            _specularPow->setInputtedData(GetWorld().lightning.specularPow);
        }
        if (Verify(_sunDirection))
        {
            _sunDirection->setInputtedData(GetWorld().lightning.sunDirection);
        }
    }

    void WorldSettingsEWC::onUpdate()
    {
    }

    void WorldSettingsEWC::onDraw()
    {
        if (_showCameraButton)
        {
            _showCameraButton->disableWidget(!gGameInstance->currentCamera);
        }
        if (_changeCameraButton)
        {
            _changeCameraButton->disableWidget(!gGameInstance->currentCamera);
        }
        if (_resetCameraButton)
        {
            _resetCameraButton->disableWidget(!gGameInstance->currentCamera);
        }

        if (_cameraInputField)
        {
            std::string str = "";
            if (gGameInstance->currentCamera)
            {
                str = gGameInstance->currentCamera->getComponentName().c_str();
            }
            _cameraInputField->setInputtedData(std::move(str));
        }

        if (Gui::CollapsingHeader("Global", ImGuiTreeNodeFlags_DefaultOpen))
        {
            _globalLayout.tick(GetWorld().getTimeDelta());
        }

        if (Gui::CollapsingHeader("Lightning", ImGuiTreeNodeFlags_DefaultOpen))
        {
            _lightningLayout.tick(GetWorld().getTimeDelta());
        }
    }
} // namespace Core