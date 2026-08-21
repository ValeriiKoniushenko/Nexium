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

#include "Animations/FrameByFrame/FrameByFrameAnimation.h"
#include "Animations/FrameByFrame/FrameByFrameAnimator.h"
#include "Scene/Rectangle.h"

#include "gtest/gtest.h"

namespace Core::Animation
{
    namespace
    {
        FrameByFrameAnimation MakeEventAnimation(const StringAtom& name, const bool loop)
        {
            FrameByFrameAnimation animation{ name };
            animation.setAtlasName("default"_atom);
            animation.setFPS(10.f);
            animation.setLoop(loop);

            for (std::size_t i = 0; i < 4; ++i)
                EXPECT_TRUE(animation.addFrame({ 0.f, 0.f }, { 1.f, 1.f }));

            auto frame = animation.getFrames()[0];
            frame.events = { "StartA"_atom, "StartB"_atom };
            EXPECT_TRUE(animation.setFrame(0, frame));

            frame = animation.getFrames()[1];
            frame.events = { "Middle"_atom };
            EXPECT_TRUE(animation.setFrame(1, frame));

            frame = animation.getFrames()[3];
            frame.events = { "EndA"_atom, "EndB"_atom };
            EXPECT_TRUE(animation.setFrame(3, frame));
            return animation;
        }

        FrameByFrameAnimator* AddAnimator(SceneObj::Rectangle& owner)
        {
            return owner.addChildComponent<FrameByFrameAnimator>("Animator"_atom);
        }
    } // namespace

    TEST(FrameByFrameAnimationTests, FrameEventMarkersRoundTripInOrder)
    {
        const Frame frame{ .name = "Contact"_atom,
                           .textureName = "player_walk_2"_atom,
                           .uvOffset = { 0.25f, 0.5f },
                           .uvSize = { 0.25f, 0.5f },
                           .events = { "Footstep"_atom, "RaiseDust"_atom } };

        const nlohmann::json serialized = frame;
        ASSERT_TRUE(serialized.contains("events"));
        ASSERT_EQ(serialized["events"].size(), 2);
        EXPECT_EQ(serialized["events"][0], "Footstep");
        EXPECT_EQ(serialized["events"][1], "RaiseDust");

        const auto restored = serialized.get<Frame>();
        EXPECT_EQ(restored.name, frame.name);
        EXPECT_EQ(restored.textureName, frame.textureName);
        ASSERT_EQ(restored.events.size(), 2);
        EXPECT_EQ(restored.events[0], "Footstep");
        EXPECT_EQ(restored.events[1], "RaiseDust");
    }

    TEST(FrameByFrameAnimationTests, LegacyFrameWithoutEventsLoadsEmptyMarkers)
    {
        Frame original;
        original.name = "Legacy frame"_atom;
        nlohmann::json legacyFrame = original;
        legacyFrame.erase("events");

        Frame restored;
        restored.events = { "StaleMarker"_atom };
        legacyFrame.get_to(restored);

        EXPECT_EQ(restored.name, "Legacy frame");
        EXPECT_TRUE(restored.events.empty());
    }

    TEST(FrameByFrameAnimationTests, AnimationRoundTripPreservesFrameMarkers)
    {
        FrameByFrameAnimation animation{ "Walk"_atom };
        animation.setAtlasName("player"_atom);
        ASSERT_TRUE(animation.addFrame({ 0.f, 0.f }, { 0.5f, 1.f }));
        ASSERT_TRUE(animation.addFrame({ 0.5f, 0.f }, { 0.5f, 1.f }));

        auto markedFrame = animation.getFrames()[1];
        markedFrame.events = { "Footstep"_atom, "EquipmentRattle"_atom };
        ASSERT_TRUE(animation.setFrame(1, markedFrame));

        const auto serialized = animation.serialize();
        FrameByFrameAnimation restored;
        RResourceStream<RJsonResourceStream> stream(serialized);
        restored.deserialize(stream);

        ASSERT_EQ(restored.getFramesCount(), 2);
        EXPECT_TRUE(restored.getFrames()[0].events.empty());
        ASSERT_EQ(restored.getFrames()[1].events.size(), 2);
        EXPECT_EQ(restored.getFrames()[1].events[0], "Footstep");
        EXPECT_EQ(restored.getFrames()[1].events[1], "EquipmentRattle");
        EXPECT_EQ(restored.getFrames()[1].name, markedFrame.name);
    }

    TEST(FrameByFrameAnimatorEventTests, EmitsEveryCrossedMarkerInOrderIncludingLoopWrap)
    {
        SceneObj::Rectangle owner{ "Owner"_atom };
        auto* animator = AddAnimator(owner);
        ASSERT_NE(animator, nullptr);
        ASSERT_TRUE(animator->addAnimation(MakeEventAnimation("Loop"_atom, true)));

        std::vector<AnimationEvent> received;
        auto subscription = animator->onEvent->subscribeAndGetID(
            [&received](const AnimationEvent& event) { received.push_back(event); });

        ASSERT_TRUE(animator->startAnimation("Loop"_atom));
        animator->tick(0.31f);
        animator->tick(0.11f);

        const std::vector<StringAtom> expectedNames{
            "StartA"_atom, "StartB"_atom, "Middle"_atom, "EndA"_atom,
            "EndB"_atom,  "StartA"_atom, "StartB"_atom,
        };
        const std::vector<std::size_t> expectedFrames{ 0, 0, 1, 3, 3, 0, 0 };

        ASSERT_EQ(received.size(), expectedNames.size());
        for (std::size_t i = 0; i < received.size(); ++i)
        {
            EXPECT_EQ(received[i].animationName, "Loop");
            EXPECT_EQ(received[i].frameIndex, expectedFrames[i]);
            EXPECT_EQ(received[i].eventName, expectedNames[i]);
        }
        EXPECT_TRUE(subscription.isValid());
    }

    TEST(FrameByFrameAnimatorEventTests, PauseResumeAndFinishDoNotDuplicateMarkers)
    {
        SceneObj::Rectangle owner{ "Owner"_atom };
        auto* animator = AddAnimator(owner);
        ASSERT_NE(animator, nullptr);
        ASSERT_TRUE(animator->addAnimation(MakeEventAnimation("Once"_atom, false)));

        std::vector<StringAtom> received;
        auto subscription = animator->onEvent->subscribeAndGetID(
            [&received](const AnimationEvent& event) { received.push_back(event.eventName); });

        ASSERT_TRUE(animator->startAnimation("Once"_atom));
        auto* animation = animator->getActiveAnimation();
        ASSERT_NE(animation, nullptr);

        animator->tick(0.05f);
        animation->pause();
        animator->tick(1.f);
        animation->resume();
        EXPECT_EQ(received, (std::vector<StringAtom>{ "StartA"_atom, "StartB"_atom }));

        animator->tick(0.41f);
        EXPECT_TRUE(animation->isFinished());
        EXPECT_EQ(received,
                  (std::vector<StringAtom>{ "StartA"_atom, "StartB"_atom, "Middle"_atom,
                                            "EndA"_atom, "EndB"_atom }));

        animation->finish();
        animator->tick(1.f);
        EXPECT_EQ(received.size(), 5);
        EXPECT_TRUE(subscription.isValid());
    }

    TEST(FrameByFrameAnimatorEventTests, RestartEmitsFrameZeroAgain)
    {
        SceneObj::Rectangle owner{ "Owner"_atom };
        auto* animator = AddAnimator(owner);
        ASSERT_NE(animator, nullptr);
        ASSERT_TRUE(animator->addAnimation(MakeEventAnimation("Restart"_atom, true)));

        std::vector<StringAtom> received;
        auto subscription = animator->onEvent->subscribeAndGetID(
            [&received](const AnimationEvent& event) { received.push_back(event.eventName); });

        ASSERT_TRUE(animator->startAnimation("Restart"_atom));
        ASSERT_TRUE(animator->startAnimation("Restart"_atom));
        EXPECT_EQ(received,
                  (std::vector<StringAtom>{ "StartA"_atom, "StartB"_atom, "StartA"_atom,
                                            "StartB"_atom }));
        EXPECT_TRUE(subscription.isValid());
    }

    TEST(FrameByFrameAnimatorEventTests, ClonesHaveIndependentSubscribers)
    {
        FrameByFrameAnimator original{ "Original"_atom };
        ASSERT_TRUE(original.addAnimation(MakeEventAnimation("CloneTest"_atom, true)));

        int originalCalls = 0;
        auto originalSubscription = original.onEvent->subscribeAndGetID(
            [&originalCalls](const AnimationEvent&) { ++originalCalls; });

        const auto clonedBase = original.clone();
        auto* cloned = dynamic_cast<FrameByFrameAnimator*>(clonedBase.get());
        ASSERT_NE(cloned, nullptr);
        EXPECT_NE(cloned->onEvent, original.onEvent);

        int clonedCalls = 0;
        auto clonedSubscription = cloned->onEvent->subscribeAndGetID(
            [&clonedCalls](const AnimationEvent&) { ++clonedCalls; });

        original.onEvent->trigger(AnimationEvent{ .animationName = "CloneTest"_atom,
                                                   .eventName = "OriginalOnly"_atom });
        EXPECT_EQ(originalCalls, 1);
        EXPECT_EQ(clonedCalls, 0);

        cloned->onEvent->trigger(AnimationEvent{ .animationName = "CloneTest"_atom,
                                                 .eventName = "CloneOnly"_atom });
        EXPECT_EQ(originalCalls, 1);
        EXPECT_EQ(clonedCalls, 1);

        const auto serialized = original.serialize();
        EXPECT_FALSE(serialized.contains("onEvent"));
        EXPECT_TRUE(originalSubscription.isValid());
        EXPECT_TRUE(clonedSubscription.isValid());
    }
} // namespace Core::Animation
