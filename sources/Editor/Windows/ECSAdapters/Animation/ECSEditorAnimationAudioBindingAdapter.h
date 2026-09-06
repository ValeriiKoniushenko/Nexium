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

#include "Audio/AnimationAudioBinding.h"
#include "Editor/Windows/NxECSBasedEditor.h"

#include <vector>

namespace Core
{
    /// @brief Properties-panel adapter that edits AnimationAudioBinding cues as an atomic draft.
    /// Changes are applied only after the user confirms the cue editor.
    CLASS();
    class ECSEditorAnimationAudioBindingAdapter final : public ECSEditorMimeAdapter
    {
        ECS_DECL(ECSEditorAnimationAudioBindingAdapter, Core::ECSEditorMimeAdapter);

    public:
        [[nodiscard]] bool canWorkWith(BaseComponent* component) const override;
        [[nodiscard]] StringAtom getProcessedAssetType() const override;
        void onEditorTargetChanged() override;

    protected:
        void onApplyAssetData(const nlohmann::json&) override {}
        void onDraw(float dt) override;

    private:
        void openCueEditor(const Audio::AnimationAudioBinding& binding);
        void drawCueEditor(Audio::AnimationAudioBinding& binding);

    private:
        std::vector<Audio::AnimationAudioCue> _draftCues;
        bool _isCueEditorOpen = false;
    };
} // namespace Core

#include "ECSEditorAnimationAudioBindingAdapter.generated.h" // added by the code generator.
