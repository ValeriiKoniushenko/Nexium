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
#include "Audio/IAudioBackend.h"
#include "Misc/BaseLog.h"

#include <cstdint>
#include <memory>
#include <vector>

namespace Core::Audio
{

    class AudioSystem final : public BaseLog
    {
    public:
        explicit AudioSystem(std::unique_ptr<IAudioBackend> backend);
        ~AudioSystem() override;

        AudioSystem(const AudioSystem&) = delete;
        AudioSystem& operator=(const AudioSystem&) = delete;
        AudioSystem(AudioSystem&&) = delete;
        AudioSystem& operator=(AudioSystem&&) = delete;

        [[nodiscard]] bool initialize();
        void shutdown();
        void update();

        VoiceHandle play(NXAudioClip clip, const PlayParams& params = {});
        bool pause(VoiceHandle voice);
        bool resume(VoiceHandle voice);
        bool stop(VoiceHandle voice);
        bool setVolume(VoiceHandle voice, float volume);
        bool setLooping(VoiceHandle voice, bool loop);

        [[nodiscard]] PlaybackState getState(VoiceHandle voice) const;
        [[nodiscard]] bool isValid(VoiceHandle voice) const;
        [[nodiscard]] bool isAvailable() const noexcept;

        void stopAll();

        [[nodiscard]] spdlog::logger* getLogger() const override;

    private:
        struct VoiceSlot
        {
            std::uint32_t generation = 1;
            IAudioBackend::VoiceId backendVoice = IAudioBackend::InvalidVoice;
            NXAudioClip clip;
            PlaybackState state = PlaybackState::Stopped;
            std::uint32_t nextFree = VoiceHandle::InvalidIndex;
            bool occupied = false;
        };

        [[nodiscard]] VoiceSlot* resolve(VoiceHandle voice) noexcept;
        [[nodiscard]] const VoiceSlot* resolve(VoiceHandle voice) const noexcept;
        [[nodiscard]] std::uint32_t allocateSlot();
        void releaseUnpublishedSlot(std::uint32_t index) noexcept;
        void retire(std::uint32_t index) noexcept;

        std::unique_ptr<IAudioBackend> _backend;
        std::vector<VoiceSlot> _voices;
        std::uint32_t _freeVoiceHead = VoiceHandle::InvalidIndex;
        bool _initialized = false;
    };

} // namespace Core::Audio
