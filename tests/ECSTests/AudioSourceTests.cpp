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

#include "Audio/AudioSource.h"

#include "gtest/gtest.h"

#include <limits>

namespace Core::Audio
{
    TEST(AudioSourceTests, UninitializedPlaybackCommandsFailSafely)
    {
        AudioSource source;

        EXPECT_FALSE(source.play().isValid());
        EXPECT_FALSE(source.pause());
        EXPECT_FALSE(source.resume());
        EXPECT_FALSE(source.stop());
        EXPECT_FALSE(source.execute(AudioCommand::Play));
        EXPECT_FALSE(source.execute(AudioCommand::Pause));
        EXPECT_FALSE(source.execute(AudioCommand::Resume));
        EXPECT_FALSE(source.execute(AudioCommand::Stop));
        EXPECT_TRUE(source.isStopped());
    }

    TEST(AudioSourceTests, NormalizesConfiguration)
    {
        AudioSource source("Music"_atom);
        EXPECT_TRUE(source.getNoTick());
        EXPECT_TRUE(source.getClipPath().isEmpty());
        EXPECT_FLOAT_EQ(source.getVolume(), 1.f);
        EXPECT_FALSE(source.isLooping());
        EXPECT_EQ(source.getMaxPolyphony(), 1);

        source.setClip("audio/music/theme.nxaudio"_atom);
        EXPECT_EQ(source.getClipPath(), "audio/music/theme.nxaudio");
        EXPECT_TRUE(source.getClipPath().isStatic());

        source.setVolume(-1.f);
        EXPECT_FLOAT_EQ(source.getVolume(), AudioSource::minVolume);
        source.setVolume(2.f);
        EXPECT_FLOAT_EQ(source.getVolume(), AudioSource::maxVolume);
        source.setVolume(std::numeric_limits<float>::quiet_NaN());
        EXPECT_FLOAT_EQ(source.getVolume(), AudioSource::maxVolume);

        source.setMaxPolyphony(0);
        EXPECT_EQ(source.getMaxPolyphony(), AudioSource::minPolyphony);
        source.setMaxPolyphony(1000);
        EXPECT_EQ(source.getMaxPolyphony(), AudioSource::maxPolyphony);
        source.setLooping(true);
        EXPECT_EQ(source.getMaxPolyphony(), 1);
        source.setMaxPolyphony(8);
        EXPECT_EQ(source.getMaxPolyphony(), 1);
        source.setLooping(false);
        source.setMaxPolyphony(4);
        EXPECT_EQ(source.getMaxPolyphony(), 4);
    }

    TEST(AudioSourceTests, SerializesOnlyConfigurationAndNormalizesOnLoad)
    {
        AudioSource source;
        source.setClip("audio/music/theme.nxaudio"_atom);
        source.setMaxPolyphony(4);

        const BaseComponent& genericSource = source;
        auto serialized = genericSource.serialize();
        EXPECT_TRUE(serialized.contains("_clipPath"));
        EXPECT_TRUE(serialized.contains("_volume"));
        EXPECT_TRUE(serialized.contains("_loop"));
        EXPECT_TRUE(serialized.contains("_maxPolyphony"));
        EXPECT_FALSE(serialized.contains("_clip"));
        EXPECT_FALSE(serialized.contains("_voices"));
        EXPECT_FALSE(serialized.contains("_backendVoice"));

        serialized["_volume"] = -3.f;
        serialized["_loop"] = true;
        serialized["_maxPolyphony"] = 999;
        serialized["_noTick"] = false;

        AudioSource restored;
        RResourceStream<RJsonResourceStream> stream(serialized);
        BaseComponent& genericRestored = restored;
        genericRestored.deserialize(stream);

        EXPECT_EQ(restored.getClipPath(), source.getClipPath());
        EXPECT_TRUE(restored.getClipPath().isStatic());
        EXPECT_FLOAT_EQ(restored.getVolume(), AudioSource::minVolume);
        EXPECT_TRUE(restored.isLooping());
        EXPECT_EQ(restored.getMaxPolyphony(), 1);
        EXPECT_TRUE(restored.getNoTick());
    }

    TEST(AudioSourceTests, IsRegisteredAndDiscoverable)
    {
        EXPECT_TRUE(TagHelper::ToTag("AnimationController") == Tag_AnimationController);
        EXPECT_TRUE(TagHelper::ToTag("AudioSource") == Tag_AudioSource);
        EXPECT_EQ(TagHelper::JoinAllToString(Tag_AnimationController | Tag_AudioSource),
                  "AnimationController,AudioSource");

        auto& factory = GetGlobalComponentFactory();
        ASSERT_TRUE(factory.containsSuchType(AudioSource::componentType));

        auto* factoryObject = factory.create(AudioSource::componentType);
        ASSERT_NE(factoryObject, nullptr);
        EXPECT_NE(dynamic_cast<AudioSource*>(factoryObject), nullptr);
        EXPECT_TRUE(factoryObject->getNoTick());

        const auto componentSearchFilter = [](const Tag tag)
        {
            return static_cast<bool>(tag & (Tag_WorldObject | Tag_AnimationController |
                                            Tag_AudioSource));
        };
        EXPECT_TRUE(componentSearchFilter(factoryObject->getTags()));
        delete factoryObject;
    }

    TEST(AudioSourceTests, CloneCopiesConfiguration)
    {
        AudioSource source;
        source.setClip("audio/sfx/hit.nxaudio"_atom);
        source.setVolume(0.25f);
        source.setMaxPolyphony(8);

        const auto clonedBase = source.clone();
        const auto* cloned = dynamic_cast<const AudioSource*>(clonedBase.get());
        ASSERT_NE(cloned, nullptr);
        EXPECT_EQ(cloned->getComponentType(), AudioSource::componentType);
        EXPECT_EQ(cloned->getClipPath(), source.getClipPath());
        EXPECT_FLOAT_EQ(cloned->getVolume(), source.getVolume());
        EXPECT_EQ(cloned->isLooping(), source.isLooping());
        EXPECT_EQ(cloned->getMaxPolyphony(), source.getMaxPolyphony());
        EXPECT_TRUE(cloned->getNoTick());
    }
} // namespace Core::Audio
