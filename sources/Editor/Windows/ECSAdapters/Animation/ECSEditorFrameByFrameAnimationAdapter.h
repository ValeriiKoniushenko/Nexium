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
#include "Core/Delegate.h"
#include "Editor/GuiComponents/VerticalLayout.h"
#include "Editor/Windows/NxECSBasedEditor.h"
#include "FrameByFrameAnimationEditor.h"

#include <unordered_map>

namespace Core
{
    namespace Gui
    {
        class Button;
    }

    CLASS();
    class ECSEditorFrameByFrameAnimationAdapter : public ECSEditorMimeAdapter
    {
        ECS_DECL(ECSEditorFrameByFrameAnimationAdapter, Core::ECSEditorMimeAdapter);

    public:
        [[nodiscard]] bool canWorkWith(BaseComponent* component) const override;
        [[nodiscard]] StringAtom getProcessedAssetType() const override;

    protected:
        void onInitialize() override;
        void onDraw(float dt) override;
        void onApplyAssetData(const nlohmann::json&) override {}

    private:
        void drawAnimationCards();

        Gui::VerticalLayout _layout;
        Gui::Button* _addAnimButton = nullptr;
        DelegateSubscriberPoolGuard _subscriptionPool;
        FrameByFrameAnimationEditor _animationEditor;
        std::unordered_map<StringAtom, FrameByFrameAnimationEditor::PreviewState> _previewStates;
    };
} // namespace Core

#include "ECSEditorFrameByFrameAnimationAdapter.generated.h"
