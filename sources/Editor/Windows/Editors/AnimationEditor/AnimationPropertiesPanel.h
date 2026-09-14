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

#pragma once

#include "Animations/FrameByFrame/FrameByFrameAnimation.h"
#include "Editor/GuiComponents/CheckBox.h"
#include "Editor/GuiComponents/Combo.h"
#include "Editor/GuiComponents/Input.h"
#include "Editor/GuiComponents/Label.h"
#include "Editor/GuiComponents/VerticalLayout.h"

namespace Core
{
    class AnimationPropertiesPanel final : public Gui::VerticalLayout
    {
    public:
        using VerticalLayout::VerticalLayout;

        void setDraft(Animation::FrameByFrameAnimation& draft) { _draft = &draft; }
        [[nodiscard]] static std::optional<Animation::Frame> frameFromPixelRect(
            glm::ivec4 rect, glm::ivec2 textureSize);

    protected:
        void onInitialize() override;
        void onDraw() override;

    private:
        void addFrame();
        void initializeFrameSource();
        void updateFrameSource();

        Gui::VerticalLayout _fields;
        Gui::VerticalLayout* _rectFields = nullptr;
        Gui::VerticalLayout* _regionFields = nullptr;

        Gui::ComboView* _atlas = nullptr;
        Gui::ComboView* _region = nullptr;
        Gui::ComboView* _frameSource = nullptr;

        Gui::Label* _atlasHint = nullptr;
        Gui::Label* _frameStatus = nullptr;

        Gui::NumInput<float>* _fps = nullptr;
        Gui::NumInput<int>* _rectX = nullptr;
        Gui::NumInput<int>* _rectY = nullptr;
        Gui::NumInput<int>* _rectWidth = nullptr;
        Gui::NumInput<int>* _rectHeight = nullptr;

        Gui::TextInput* _name = nullptr;
        Gui::CheckBox* _loop = nullptr;

        StringAtom _regionAtlas;
        Animation::FrameByFrameAnimation* _draft = nullptr;
        DelegateSubscriber _addFrameSubscription;
    };

} // namespace Core
