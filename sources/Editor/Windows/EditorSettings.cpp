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
#include "Editor/GuiComponents/Input.h"
#include "Editor/GuiComponents/Label.h"
#include "GameplaySystem/Framework/GameInstance.h"

namespace Core
{
    ECS_REGISTER_NEW_COMPONENT_TYPE(EditorSettingsEWC)

    void EditorSettingsEWC::onInitialize()
    {
        BaseFloatEWC::onInitialize();

        _layout.setVerticalAlign(VerticalLayout::Align::Top);
        _layout.setHorizontalAlign(VerticalLayout::Align::Left);

        auto* horizontal = _layout.addChildComponent<HorizontalLayout>();
        horizontal->setHorizontalAlign(HorizontalLayout::Align::Center);
        horizontal->setVerticalAlign(HorizontalLayout::Align::Center);
        horizontal->addChildComponent<Button>()->setText("Button 1");
        horizontal->addChildComponent<Button>()->setText("Hello world!").setHeight(50.f);
        horizontal->addChildComponent<Button>()->setText("Button 2");

        _layout.addChildComponent<Button>()->setText("Click me");

        _layout.addChildComponent<Button>()->setText("Vertical layout");

        auto line = HorizontalLayout::Create();

        auto labelAndButton = HorizontalLayout::Create();
        labelAndButton->setFitContent(true);
        (void)labelAndButton->addChildComponent<Label>("Some text: ");
        (void)labelAndButton->addChildComponent<Button>("Click me!");
        line->attachChild(labelAndButton);
        line->attachChild(labelAndButton);

        _layout.attachChild(line);

        *_layout.addChildComponent<HorizontalLayout>() = *horizontal;

        auto search = HorizontalLayout::Create();
        (void)search->addChildComponent<Label>("Search:");
        (void)search->addChildComponent<Input>();
        _layout.attachChild(search);
    }

    void EditorSettingsEWC::onDraw()
    {
        const float tick = gGameInstance->world.timeDelta;

        _layout.tick(tick);
    }
} // namespace Core
