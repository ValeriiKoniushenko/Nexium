// MIT License
//
// Copyright (c) 2019-2025 Valerii Koniushenko
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "EditorSettings.h"

#include "Editor/GuiComponents/Button.h"
#include "Editor/GuiComponents/HorizontalLayout.h"
#include "GameplaySystem/Framework/GameInstance.h"

namespace Core
{
    ECS_REGISTER_NEW_COMPONENT_TYPE(EditorSettingsEWC)

    void EditorSettingsEWC::onInitialize()
    {
        BaseFloatEWC::onInitialize();

        //        _layout.setSpacing(40);
        _layout.setHeight(300);
        _layout.setVerticalAlign(VerticalLayout::Align::Center);

        auto* horizontal = _layout.addChildComponent<HorizontalLayout>();
        horizontal->setHorizontalAlign(HorizontalLayout::Align::Center);
        horizontal->setVerticalAlign(HorizontalLayout::Align::Center);
        horizontal->setWidth(300.f);
        horizontal->addChildComponent<Button>()
            ->setText("Button 1")
            .onClick.subscribe(
                [](Button* context)
                {
                    static int i = 0;
                    if (++i > 2)
                    {
                        i = 0;
                    }

                    static HorizontalLayout::Align aligns[3]
                        = { HorizontalLayout::Align::Top, HorizontalLayout::Align::Center,
                            HorizontalLayout::Align::Bottom };
                    context->getParent()->castTo<HorizontalLayout>()->setVerticalAlign(aligns[i]);
                });
        horizontal->addChildComponent<Button>()
            ->setText("Hello world!")
            .setHeight(50.f)
            .onClick.subscribe(
                [](Button* context)
                {
                    static int i = 0;
                    if (++i > 2)
                    {
                        i = 0;
                    }

                    static HorizontalLayout::Align aligns[3]
                        = { HorizontalLayout::Align::Left, HorizontalLayout::Align::Center,
                            HorizontalLayout::Align::Right };
                    context->getParent()->castTo<HorizontalLayout>()->setHorizontalAlign(aligns[i]);
                });
        horizontal->addChildComponent<Button>()->setText("Button 2");
        horizontal->addChildComponent<Button>()->setText("X").onClick.subscribe(
            [](Button* context)
            {
                auto horizontal = context->getParent()->castTo<HorizontalLayout>();
                horizontal->setIsDrawOutline(!horizontal->getIsDrawOutline());
            });

        *_layout.addChildComponent<HorizontalLayout>() = *horizontal;
    }

    void EditorSettingsEWC::onDraw()
    {
        const float tick = gGameInstance->world.timeDelta;

        _layout.tick(tick);
    }
} // namespace Core
