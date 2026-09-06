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

#include "Audio/AnimationAudioBinding.h"

#include "Animations/FrameByFrame/FrameByFrameAnimator.h"
#include "Audio/AudioSource.h"

namespace Core::Audio
{
    bool AnimationAudioCue::isValid() const noexcept
    {
        if (animationName.isEmpty() || eventName.isEmpty() || audioSourceName.isEmpty())
            return false;

        switch (command)
        {
            case AudioCommand::Play:
            case AudioCommand::Pause:
            case AudioCommand::Resume:
            case AudioCommand::Stop:
                return true;
        }

        return false;
    }

    void to_json(nlohmann::json& json, const AnimationAudioCue& cue)
    {
        json = R<AnimationAudioCue>::Serialize(cue).getData();
    }

    void from_json(const nlohmann::json& json, AnimationAudioCue& cue)
    {
        const RResourceStream<RJsonResourceStream> stream(json);
        R<AnimationAudioCue>::Deserialize(stream, cue);
    }

    ECS_IMPL(AnimationAudioBinding);

    AnimationAudioBinding::AnimationAudioBinding(const AnimationAudioBinding& other)
        : BaseComponent(other),
          _cues(other._cues)
    {
    }

    AnimationAudioBinding& AnimationAudioBinding::operator=(
        const AnimationAudioBinding& other)
    {
        if (this == &other)
            return *this;

        _subscriptions.clearAndReleaseAll();
        BaseComponent::operator=(other);
        _cues = other._cues;
        _warnedMissingSources.clear();
        _warnedMissingAnimator = false;

        if (isInitialized())
            subscribeToOwnerAnimator();

        return *this;
    }

    bool AnimationAudioBinding::addCue(const AnimationAudioCue& cue)
    {
        if (!cue.isValid())
            return false;

        _cues.push_back(cue);
        return true;
    }

    bool AnimationAudioBinding::removeCue(const std::size_t index)
    {
        if (index >= _cues.size())
            return false;

        _cues.erase(_cues.begin() + static_cast<std::ptrdiff_t>(index));
        return true;
    }

    void AnimationAudioBinding::clearCues() noexcept
    {
        _cues.clear();
    }

    Tag AnimationAudioBinding::getTags() const
    {
        return BaseComponent::getTags() | Tag_AnimationController;
    }

    void AnimationAudioBinding::onInitialize()
    {
        subscribeToOwnerAnimator();
    }

    void AnimationAudioBinding::subscribeToOwnerAnimator()
    {
        _subscriptions.clearAndReleaseAll();

        auto* owner = getOwner();
        auto* animator
            = owner ? owner->findFirstChildOf<Animation::FrameByFrameAnimator>() : nullptr;
        if (!animator || !animator->onEvent)
        {
            if (!_warnedMissingAnimator)
            {
                warnLog("AnimationAudioBinding '{}' can't resolve a FrameByFrameAnimator under "
                        "its owner."_f
                        << getComponentName());
                _warnedMissingAnimator = true;
            }
            return;
        }

        _warnedMissingAnimator = false;
        _subscriptions << animator->onEvent->subscribeAndGetID(
            [this](const Animation::AnimationEvent& event) { handleAnimationEvent(event); });
    }

    void AnimationAudioBinding::handleAnimationEvent(const Animation::AnimationEvent& event)
    {
        auto* owner = getOwner();
        if (!owner)
            return;

        for (const auto& cue : _cues)
        {
            if (!cue.isValid() || cue.animationName != event.animationName
                || cue.eventName != event.eventName)
            {
                continue;
            }

            auto* source = owner->findFirstChildOf<AudioSource>(cue.audioSourceName);
            if (!source)
            {
                if (_warnedMissingSources.emplace(cue.audioSourceName).second)
                {
                    warnLog("AnimationAudioBinding '{}' can't resolve AudioSource '{}'."_f
                            << getComponentName() << cue.audioSourceName);
                }
                continue;
            }

            _warnedMissingSources.erase(cue.audioSourceName);
            (void)source->execute(cue.command);
        }
    }
} // namespace Core::Audio
