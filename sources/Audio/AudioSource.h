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

#include "AssetsManager/AudioClipAsset.h"
#include "ECS/BaseComponent.h"

#include <cstdint>
#include <vector>

namespace Core::Audio
{
    CLASS();
    class AudioSource final : public BaseComponent
    {
        ECS_DECL_NO_CNSTR(AudioSource, Core::BaseComponent);

    public:
        inline static constexpr float minVolume = 0.f;
        inline static constexpr float maxVolume = 1.f;
        inline static constexpr std::uint32_t minPolyphony = 1;
        inline static constexpr std::uint32_t maxPolyphony = 32;

        explicit AudioSource(const StringAtom& name = ""_atom);
        AudioSource(const AudioSource& other);
        AudioSource(AudioSource&& other) noexcept;
        AudioSource& operator=(const AudioSource& other);
        AudioSource& operator=(AudioSource&& other) noexcept;
        ~AudioSource() override;

        VoiceHandle play();
        bool pause();
        bool resume();
        bool stop();
        bool execute(AudioCommand command);

        void setClip(const StringAtom& logicalPath);
        void setVolume(float volume);
        void setLooping(bool loop);
        void setMaxPolyphony(std::uint32_t maxPolyphony);

        [[nodiscard]] const StringAtom& getClipPath() const noexcept { return _clipPath; }
        [[nodiscard]] float getVolume() const noexcept { return _volume; }
        [[nodiscard]] bool isLooping() const noexcept { return _loop; }
        [[nodiscard]] std::uint32_t getMaxPolyphony() const noexcept { return _maxPolyphony; }

        [[nodiscard]] PlaybackState getPlaybackState() const;
        [[nodiscard]] bool isPlaying() const;
        [[nodiscard]] bool isPaused() const;
        [[nodiscard]] bool isStopped() const;

        [[nodiscard]] Tag getTags() const override;
        void onPostDeserialize(AbstractComponent* obj, const RLogsCollector& logs) override;

    protected:
        void onInitialize() override;

    private:
        void normalizeConfiguration();
        void resolveConfiguredClip();
        void pruneInvalidVoices() const;
        void stopOldestVoice();

        FIELD();
        StringAtom _clipPath;

        FIELD();
        float _volume = 1.f;

        FIELD();
        bool _loop = false;

        FIELD();
        std::uint32_t _maxPolyphony = 1;

        NXAudioClip _clip;
        mutable std::vector<VoiceHandle> _voices;
    };
} // namespace Core::Audio

#include "AudioSource.generated.h" // added by the code generator. Better don't move it.
