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

#include "Animations/AnimationEvent.h"
#include "Audio/AudioTypes.h"
#include "Core/Delegate.h"
#include "ECS/BaseComponent.h"

#include <cstddef>
#include <unordered_set>
#include <vector>

namespace Core::Audio
{
    CLASS();
    struct AnimationAudioCue
    {
        R_FRIEND(AnimationAudioCue);

        [[nodiscard]] bool isValid() const noexcept;

        FIELD();
        StringAtom animationName;

        FIELD();
        StringAtom eventName;

        FIELD();
        StringAtom audioSourceName;

        FIELD();
        Core::Audio::AudioCommand command = Core::Audio::AudioCommand::Play;
    };

    void to_json(nlohmann::json& json, const AnimationAudioCue& cue);
    void from_json(const nlohmann::json& json, AnimationAudioCue& cue);

    CLASS();
    class AnimationAudioBinding final : public BaseComponent
    {
        ECS_DECL(AnimationAudioBinding, Core::BaseComponent);

    public:
        AnimationAudioBinding(const AnimationAudioBinding& other);
        AnimationAudioBinding& operator=(const AnimationAudioBinding& other);

        bool addCue(const AnimationAudioCue& cue);
        bool removeCue(std::size_t index);
        void clearCues() noexcept;

        [[nodiscard]] const std::vector<AnimationAudioCue>& getCues() const noexcept
        {
            return _cues;
        }

    protected:
        void onInitialize() override;

    private:
        void subscribeToOwnerAnimator();
        void handleAnimationEvent(const Animation::AnimationEvent& event);

        FIELD();
        std::vector<AnimationAudioCue> _cues;

        DelegateSubscriberPoolGuard _subscriptions;
        std::unordered_set<StringAtom> _warnedMissingSources;
        bool _warnedMissingAnimator = false;
    };
} // namespace Core::Audio

#include "AnimationAudioBinding.generated.h" // added by the code generator. Better don't move it.
