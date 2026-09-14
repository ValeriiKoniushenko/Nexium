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

#include "NxWorld/Animations/FrameByFrame/FrameByFrameAnimation.h"
#include "Editor/GuiComponents/Button.h"
#include "Editor/GuiComponents/HorizontalLayout.h"
#include "Editor/GuiComponents/Label.h"
#include "Editor/GuiComponents/VerticalLayout.h"

namespace NX
{
    class AnimationPreviewPanel final : public NX::Gui::VerticalLayout
    {
    public:
        using VerticalLayout::VerticalLayout;
        void setDraft(const Animation::FrameByFrameAnimation& draft);

    protected:
        void onInitialize() override;
        void onDraw() override;

    private:
        void synchronizeDraft();
        [[nodiscard]] std::size_t currentFrameIndex() const;
        void drawCanvas(glm::vec2 size);

        const Animation::FrameByFrameAnimation* _draft = nullptr;
        Animation::FrameByFrameAnimation _preview;
        Gui::HorizontalLayout _controls;
        Gui::Button* _play = nullptr;
        Gui::Button* _restart = nullptr;
        Gui::Label* _position = nullptr;
        Gui::Label _title;
        DelegateSubscriberPoolGuard _subscriptions;
    };

} // namespace NX
