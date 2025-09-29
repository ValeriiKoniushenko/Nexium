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
            _lightColor = h->addChildComponent<Gui::Color3Input>();
            _lightColor->setInputtedData(GetWorld().lightning.color);
            _lightColor->onInput.subscribe(
                [](Color3 color)
                {
                    GetWorld().lightning.color = color.toNorm();
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