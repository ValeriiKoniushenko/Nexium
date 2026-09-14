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

#include "AnimationEditor.h"

#include "AnimationFramesPanel.h"
#include "AnimationPreviewPanel.h"
#include "AnimationPropertiesPanel.h"

namespace NX
{
    void AnimationEditorEWC::onInitialize()
    {
        BaseFloatEWC::onInitialize();

        _minWindowSize = FSize2(800.f, 600.f);

        _layout.setComponentName("Animation editor"_atom);
        _layout.setFlex(Gui::Flex::FlexWidth);

        auto* content = _layout.addChildComponent<Gui::HorizontalLayout>("Animation content"_atom);
        content->setFlex(Gui::Flex::FlexWidth);
        content->setHorizontalAlign(Gui::Align::Left);
        content->setVerticalAlign(Gui::Align::Top);

        auto* preview = content->addChildComponent<AnimationPreviewPanel>("Animation preview"_atom);
        preview->setFlex(Gui::Flex::FlexWidth);
        preview->setHeight(400.f);
        preview->setDraft(_draft);

        auto* properties
            = content->addChildComponent<AnimationPropertiesPanel>("Animation properties"_atom);
        properties->setFlex(Gui::Flex::Fixed);
        properties->setWidth(300.f);
        properties->setHeight(400.f);
        properties->setDraft(_draft);
        _frameCount = _layout.addChildComponent<Gui::Label>("Animation frame count"_atom);
        _frameCount->setText("0 frames"_atom);
        auto* frames = _layout.addChildComponent<AnimationFramesPanel>("Animation frames"_atom);
        frames->setDraft(_draft);
        frames->setFlex(Gui::Flex::FlexWidth);
        frames->setHeight(180.f);
    }

    void AnimationEditorEWC::onDraw()
    {
        _frameCount->setText(StringAtom::MakeFrom(_draft.getFramesCount()) + " frames"_atom);
        _layout.tick(ImGui::GetIO().DeltaTime);
    }

} // namespace NX
