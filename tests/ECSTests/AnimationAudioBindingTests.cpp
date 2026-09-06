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
#include "Misc/ITagHolder.h"
#include "Scene/Rectangle.h"
#include "gtest/gtest.h"
#include "spdlog/logger.h"
#include "spdlog/sinks/ostream_sink.h"

#include <algorithm>
#include <sstream>

namespace Core::Audio
{
    namespace
    {
        class ScopedLogCapture final
        {
        public:
            explicit ScopedLogCapture(spdlog::logger* logger)
                : _logger(logger),
                  _sink(std::make_shared<spdlog::sinks::ostream_sink_mt>(_output))
            {
                _logger->sinks().push_back(_sink);
            }

            ~ScopedLogCapture()
            {
                std::erase(_logger->sinks(), _sink);
            }

            [[nodiscard]] std::string getOutput()
            {
                _sink->flush();
                return _output.str();
            }

        private:
            spdlog::logger* _logger;
            std::ostringstream _output;
            std::shared_ptr<spdlog::sinks::ostream_sink_mt> _sink;
        };

        AnimationAudioCue MakeCue(const AudioCommand command = AudioCommand::Play)
        {
            return AnimationAudioCue{ .animationName = "Walk"_atom,
                                      .eventName = "Footstep"_atom,
                                      .audioSourceName = "FootstepSound"_atom,
                                      .command = command };
        }

        std::size_t CountOccurrences(const std::string& text, const std::string_view pattern)
        {
            std::size_t count = 0;
            for (auto pos = text.find(pattern); pos != std::string::npos;
                 pos = text.find(pattern, pos + pattern.size()))
            {
                ++count;
            }
            return count;
        }
    } // namespace

    TEST(AnimationAudioBindingTests, ValidatesAndMutatesCueCollection)
    {
        AnimationAudioBinding binding;

        auto cue = MakeCue();
        EXPECT_TRUE(cue.isValid());
        EXPECT_TRUE(binding.addCue(cue));
        ASSERT_EQ(binding.getCues().size(), 1);

        cue.animationName = StringAtom{};
        EXPECT_FALSE(cue.isValid());
        EXPECT_FALSE(binding.addCue(cue));

        cue = MakeCue();
        cue.eventName = StringAtom{};
        EXPECT_FALSE(binding.addCue(cue));

        cue = MakeCue();
        cue.audioSourceName = StringAtom{};
        EXPECT_FALSE(binding.addCue(cue));

        cue = MakeCue(static_cast<AudioCommand>(255));
        EXPECT_FALSE(cue.isValid());
        EXPECT_FALSE(binding.addCue(cue));

        EXPECT_FALSE(binding.removeCue(1));
        EXPECT_TRUE(binding.removeCue(0));
        EXPECT_TRUE(binding.getCues().empty());

        EXPECT_TRUE(binding.addCue(MakeCue(AudioCommand::Pause)));
        EXPECT_TRUE(binding.addCue(MakeCue(AudioCommand::Stop)));
        binding.clearCues();
        EXPECT_TRUE(binding.getCues().empty());
    }

    TEST(AnimationAudioBindingTests, SerializesAndReloadsCueConfiguration)
    {
        AnimationAudioBinding binding("PlayerAudioBinding"_atom);
        ASSERT_TRUE(binding.addCue(MakeCue(AudioCommand::Play)));

        auto stopCue = MakeCue(AudioCommand::Stop);
        stopCue.animationName = "Run"_atom;
        stopCue.eventName = "Finished"_atom;
        stopCue.audioSourceName = "MovementSound"_atom;
        ASSERT_TRUE(binding.addCue(stopCue));

        const BaseComponent& genericBinding = binding;
        const auto serialized = genericBinding.serialize();

        ASSERT_TRUE(serialized.contains("_cues"));
        ASSERT_EQ(serialized["_cues"].size(), 2);
        EXPECT_EQ(serialized["_cues"][0]["animationName"], "Walk");
        EXPECT_EQ(serialized["_cues"][0]["eventName"], "Footstep");
        EXPECT_EQ(serialized["_cues"][0]["audioSourceName"], "FootstepSound");
        EXPECT_TRUE(serialized["_cues"][0].contains("command"));
        EXPECT_FALSE(serialized.contains("_subscriptions"));
        EXPECT_FALSE(serialized.contains("_warnedMissingSources"));
        EXPECT_FALSE(serialized.contains("_warnedMissingAnimator"));
        EXPECT_FALSE(serialized.contains("onEvent"));

        AnimationAudioBinding restored;
        RResourceStream<RJsonResourceStream> stream(serialized);
        BaseComponent& genericRestored = restored;
        genericRestored.deserialize(stream);

        ASSERT_EQ(restored.getCues().size(), 2);
        EXPECT_EQ(restored.getCues()[0].animationName, "Walk");
        EXPECT_EQ(restored.getCues()[0].eventName, "Footstep");
        EXPECT_EQ(restored.getCues()[0].audioSourceName, "FootstepSound");
        EXPECT_EQ(restored.getCues()[0].command, AudioCommand::Play);
        EXPECT_TRUE(restored.getCues()[0].isValid());
        EXPECT_EQ(restored.getCues()[1].animationName, "Run");
        EXPECT_EQ(restored.getCues()[1].eventName, "Finished");
        EXPECT_EQ(restored.getCues()[1].audioSourceName, "MovementSound");
        EXPECT_EQ(restored.getCues()[1].command, AudioCommand::Stop);
        EXPECT_TRUE(restored.getCues()[1].isValid());
    }

    TEST(AnimationAudioBindingTests, IsRegisteredWithComponentFactory)
    {
        auto& factory = GetGlobalComponentFactory();
        ASSERT_TRUE(factory.containsSuchType(AnimationAudioBinding::componentType));

        auto* factoryObject = factory.create(AnimationAudioBinding::componentType);
        ASSERT_NE(factoryObject, nullptr);
        EXPECT_NE(dynamic_cast<AnimationAudioBinding*>(factoryObject), nullptr);
        delete factoryObject;
    }

    TEST(AnimationAudioBindingTests, IsDiscoverableAlongsideAnimationControllers)
    {
        AnimationAudioBinding binding;

        EXPECT_NE(binding.getTags() & Tag_AnimationController, Tag_None);
    }

    TEST(AnimationAudioBindingTests, SubscribesAndUnsubscribesWithComponentLifetime)
    {
        SceneObj::Rectangle owner("Owner"_atom);
        auto* animator
            = owner.addChildComponent<Animation::FrameByFrameAnimator>("Animator"_atom);
        ASSERT_NE(animator, nullptr);
        EXPECT_EQ(animator->onEvent->getSubscriptionsCount(), 0);

        auto* binding = owner.addChildComponent<AnimationAudioBinding>("AudioBinding"_atom);
        ASSERT_NE(binding, nullptr);
        EXPECT_EQ(animator->onEvent->getSubscriptionsCount(), 1);

        ASSERT_TRUE(owner.removeChild(binding));
        EXPECT_EQ(animator->onEvent->getSubscriptionsCount(), 0);
        animator->onEvent->trigger(Animation::AnimationEvent{ .animationName = "Walk"_atom,
                                                               .frameIndex = 1,
                                                               .eventName = "Footstep"_atom });
    }

    TEST(AnimationAudioBindingTests, MatchesExactlyAndSuppressesRepeatedMissingSourceWarnings)
    {
        SceneObj::Rectangle owner("Owner"_atom);
        auto* animator
            = owner.addChildComponent<Animation::FrameByFrameAnimator>("Animator"_atom);
        ASSERT_NE(animator, nullptr);

        auto* binding = owner.addChildComponent<AnimationAudioBinding>("AudioBinding"_atom);
        ASSERT_NE(binding, nullptr);
        ASSERT_TRUE(binding->addCue(MakeCue()));

        ScopedLogCapture capture(binding->getLogger());
        animator->onEvent->trigger(Animation::AnimationEvent{ .animationName = "Run"_atom,
                                                               .frameIndex = 1,
                                                               .eventName = "Footstep"_atom });
        animator->onEvent->trigger(Animation::AnimationEvent{ .animationName = "Walk"_atom,
                                                               .frameIndex = 1,
                                                               .eventName = "Other"_atom });
        EXPECT_EQ(CountOccurrences(capture.getOutput(), "can't resolve AudioSource"), 0);

        const auto matchingEvent
            = Animation::AnimationEvent{ .animationName = "Walk"_atom,
                                         .frameIndex = 1,
                                         .eventName = "Footstep"_atom };
        animator->onEvent->trigger(matchingEvent);
        animator->onEvent->trigger(matchingEvent);
        EXPECT_EQ(CountOccurrences(capture.getOutput(), "can't resolve AudioSource"), 1);

        auto* source = owner.addChildComponent<AudioSource>("FootstepSound"_atom);
        ASSERT_NE(source, nullptr);
        animator->onEvent->trigger(matchingEvent);
        EXPECT_EQ(CountOccurrences(capture.getOutput(), "can't resolve AudioSource"), 1);

        ASSERT_TRUE(owner.removeChild(source));
        animator->onEvent->trigger(matchingEvent);
        EXPECT_EQ(CountOccurrences(capture.getOutput(), "can't resolve AudioSource"), 2);
    }

    TEST(AnimationAudioBindingTests, MissingAnimatorWarningDoesNotRepeatAcrossReinitialization)
    {
        AnimationAudioBinding binding("AudioBinding"_atom);
        ScopedLogCapture capture(binding.getLogger());

        binding.initialize();
        binding.invalidate();
        binding.initialize();

        EXPECT_EQ(CountOccurrences(capture.getOutput(), "can't resolve a FrameByFrameAnimator"),
                  1);
    }

    TEST(AnimationAudioBindingTests, ClonedOwnersReceiveIndependentSubscriptions)
    {
        SceneObj::Rectangle owner("Owner"_atom);
        auto* animator
            = owner.addChildComponent<Animation::FrameByFrameAnimator>("Animator"_atom);
        ASSERT_NE(animator, nullptr);
        auto* binding = owner.addChildComponent<AnimationAudioBinding>("AudioBinding"_atom);
        ASSERT_NE(binding, nullptr);
        ASSERT_TRUE(binding->addCue(MakeCue(AudioCommand::Stop)));
        ASSERT_EQ(animator->onEvent->getSubscriptionsCount(), 1);

        auto clonedBase = owner.clone();
        auto* clonedOwner = dynamic_cast<SceneObj::Rectangle*>(clonedBase.get());
        ASSERT_NE(clonedOwner, nullptr);
        auto* clonedAnimator
            = clonedOwner->findFirstChildOf<Animation::FrameByFrameAnimator>("Animator"_atom);
        auto* clonedBinding
            = clonedOwner->findFirstChildOf<AnimationAudioBinding>("AudioBinding"_atom);
        ASSERT_NE(clonedAnimator, nullptr);
        ASSERT_NE(clonedBinding, nullptr);
        ASSERT_EQ(clonedBinding->getCues().size(), 1);
        EXPECT_EQ(clonedAnimator->onEvent->getSubscriptionsCount(), 0);
        EXPECT_EQ(animator->onEvent->getSubscriptionsCount(), 1);

        clonedOwner->initialize();
        EXPECT_EQ(clonedAnimator->onEvent->getSubscriptionsCount(), 1);
        EXPECT_EQ(animator->onEvent->getSubscriptionsCount(), 1);

        clonedBase = {};
        EXPECT_EQ(animator->onEvent->getSubscriptionsCount(), 1);
    }

    TEST(AnimationAudioBindingTests, CopyAssignmentRebindsInitializedDestination)
    {
        SceneObj::Rectangle owner("Owner"_atom);
        auto* animator
            = owner.addChildComponent<Animation::FrameByFrameAnimator>("Animator"_atom);
        ASSERT_NE(animator, nullptr);
        auto* destination
            = owner.addChildComponent<AnimationAudioBinding>("Destination"_atom);
        ASSERT_NE(destination, nullptr);
        ASSERT_EQ(animator->onEvent->getSubscriptionsCount(), 1);

        AnimationAudioBinding configuration("ConfiguredBinding"_atom);
        ASSERT_TRUE(configuration.addCue(MakeCue(AudioCommand::Resume)));
        *destination = configuration;

        ASSERT_TRUE(destination->isInitialized());
        ASSERT_EQ(destination->getCues().size(), 1);
        EXPECT_EQ(destination->getCues()[0].command, AudioCommand::Resume);
        EXPECT_EQ(animator->onEvent->getSubscriptionsCount(), 1);

        AnimationAudioBinding copied(configuration);
        EXPECT_FALSE(copied.isInitialized());
        ASSERT_EQ(copied.getCues().size(), 1);
        EXPECT_EQ(animator->onEvent->getSubscriptionsCount(), 1);
    }
} // namespace Core::Audio
