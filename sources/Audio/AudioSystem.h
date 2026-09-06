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
    /// @brief GameInstance-owned service that maps safe VoiceHandles to backend playback voices.
    ///
    /// The system retains an AudioClipAsset for each active voice, polls natural completion in
    /// update(), and recycles slots through generation-checked handles. It is not serializable.
    class AudioSystem final : public BaseLog
    {
    public:
        explicit AudioSystem(std::unique_ptr<IAudioBackend> backend);
        ~AudioSystem() override;

        AudioSystem(const AudioSystem&) = delete;
        AudioSystem& operator=(const AudioSystem&) = delete;
        AudioSystem(AudioSystem&&) = delete;
        AudioSystem& operator=(AudioSystem&&) = delete;

        /// @brief Initialize the configured backend.
        /// @return `true` when the service can accept playback commands.
        [[nodiscard]] bool initialize();
        /// @brief Stop all voices and release the backend. Safe to call more than once.
        void shutdown();
        /// @brief Retire naturally completed non-looping voices.
        /// Call once per main-loop tick after game and editor processing.
        void update();

        /// @brief Start an independently controlled playback voice for a ready audio clip.
        /// @param clip Strong clip reference retained for the lifetime of the voice.
        /// @param params Initial gain and looping policy.
        /// @return A valid voice handle, or an invalid handle when playback cannot start.
        VoiceHandle play(NXAudioClip clip, const PlayParams& params = {});
        /// @brief Pause one valid playing voice.
        /// @param voice Voice handle returned by play().
        /// @return `true` on success; `false` for an invalid handle or failed backend call.
        bool pause(VoiceHandle voice);
        /// @brief Resume one valid paused voice.
        /// @param voice Voice handle returned by play().
        /// @return `true` on success; `false` for an invalid handle or failed backend call.
        bool resume(VoiceHandle voice);
        /// @brief Stop and destroy one valid voice, invalidating its handle.
        /// @param voice Voice handle returned by play().
        /// @return `true` when the voice was stopped and retired.
        bool stop(VoiceHandle voice);
        /// @brief Change the gain of one valid voice.
        /// @param voice Voice handle to update.
        /// @param volume New linear gain.
        /// @return `true` when the backend accepted the change.
        bool setVolume(VoiceHandle voice, float volume);
        /// @brief Change the looping policy of one valid voice.
        /// @param voice Voice handle to update.
        /// @param loop New looping policy.
        /// @return `true` when the backend accepted the change.
        bool setLooping(VoiceHandle voice, bool loop);

        /// @brief Get the current state of a voice.
        /// @param voice Voice handle to query.
        /// @return PlaybackState::Stopped for an invalid handle.
        [[nodiscard]] PlaybackState getState(VoiceHandle voice) const;
        /// @brief Check whether a handle resolves to an occupied voice slot.
        /// @param voice Voice handle to query.
        /// @return `true` only while the referenced voice remains active.
        [[nodiscard]] bool isValid(VoiceHandle voice) const;
        /// @brief Check whether the backend was initialized successfully.
        /// @return `true` when the service can accept playback commands.
        [[nodiscard]] bool isAvailable() const noexcept;

        /// @brief Stop and destroy every active voice without shutting down the output device.
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
