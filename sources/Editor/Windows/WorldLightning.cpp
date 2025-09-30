/*
 * MIT License
 *
 * Copyright (c) 2018-2025 Valerii Koniushenko
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

#include "WorldLightning.h"

#include "Editor/GuiComponents/HorizontalLayout.h"
#include "Editor/GuiComponents/Label.h"
#include "Editor/GuiComponents/VecInput.h"
#include "GameplaySystem/Framework/GameInstance.h"

namespace Core
{
    ECS_COMPONENT_IMPL(WorldLightningEWC)

    void WorldLightningEWC::onInitialize()
    {
        BaseFloatEWC::onInitialize();

        {
            auto* h = _layout.addChildComponent<Gui::HorizontalLayout>();
            h->setHorizontalAlign(Gui::Align::SpaceBetween);
            h->addChildComponent<Gui::Label>("Light color");
            auto* input = h->addChildComponent<Gui::Color3Input>();
            input->setInputtedData(GetWorld().lightning.color);
            input->onInput.subscribe(
                [](Color3 color)
                {
                    GetWorld().lightning.color = color.toNorm();
                });
        }
        {
            auto* h = _layout.addChildComponent<Gui::HorizontalLayout>();
            h->setHorizontalAlign(Gui::Align::SpaceBetween);
            h->addChildComponent<Gui::Label>("Ambient strength");
            auto* input = h->addChildComponent<Gui::FloatInput>();
            input->setInputtedData(GetWorld().lightning.ambientStrength);
            input->onInput.subscribe(
                [](float value)
                {
                    GetWorld().lightning.ambientStrength = value;
                });
        }
        {
            auto* h = _layout.addChildComponent<Gui::HorizontalLayout>();
            h->setHorizontalAlign(Gui::Align::SpaceBetween);
            h->addChildComponent<Gui::Label>("Min light strength");
            auto* input = h->addChildComponent<Gui::FloatInput>();
            input->setInputtedData(GetWorld().lightning.minLightStrength);
            input->onInput.subscribe(
                [](float value)
                {
                    GetWorld().lightning.minLightStrength = value;
                });
        }
        {
            auto* h = _layout.addChildComponent<Gui::HorizontalLayout>();
            h->setHorizontalAlign(Gui::Align::SpaceBetween);
            h->addChildComponent<Gui::Label>("Specular strength");
            auto* input = h->addChildComponent<Gui::FloatInput>();
            input->setInputtedData(GetWorld().lightning.specularStrength);
            input->onInput.subscribe(
                [](float value)
                {
                    GetWorld().lightning.specularStrength = value;
                });
        }
        {
            auto* h = _layout.addChildComponent<Gui::HorizontalLayout>();
            h->setHorizontalAlign(Gui::Align::SpaceBetween);
            h->addChildComponent<Gui::Label>("Specular pow");
            auto* input = h->addChildComponent<Gui::FloatInput>();
            input->setInputtedData(GetWorld().lightning.specularPow);
            input->onInput.subscribe(
                [](float value)
                {
                    GetWorld().lightning.specularPow = value;
                });
        }
        {
            auto* h = _layout.addChildComponent<Gui::HorizontalLayout>();
            h->setHorizontalAlign(Gui::Align::SpaceBetween);
            h->addChildComponent<Gui::Label>("Sun light direction");
            auto* input = h->addChildComponent<Gui::Float3Input>();
            input->setFlex(Gui::Flex::Fixed);
            input->setWidth(240.f);

            for (auto* i : input->inputs)
            {
                i->setMin(-1.f);
                i->setMax(1.f);
                i->setStep(.01f);
            }

            input->setInputtedData(GetWorld().lightning.sunDirection);
            input->onInput.subscribe(
                [](glm::vec3 value)
                {
                    GetWorld().lightning.sunDirection = value;
                });
        }
    }

    void WorldLightningEWC::onUpdate()
    {
    }

    void WorldLightningEWC::onDraw()
    {
        _layout.tick(GetWorld().timeDelta);
    }
} // namespace Core