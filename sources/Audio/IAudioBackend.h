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

#include "AudioTypes.h"

#include <cstdint>
#include <limits>

namespace Core::Audio
{
    /// @brief Backend boundary used by AudioSystem to manage independent playback cursors.
    ///
    /// Implementations own native device objects and voice resources. Calls are made from the
    /// main thread; this interface must not expose native backend types to the rest of Nexium.
    class IAudioBackend
    {
    public:
        /// @brief Backend-local voice identifier without generation semantics.
        using VoiceId = std::uint32_t;
        static constexpr VoiceId InvalidVoice = std::numeric_limits<VoiceId>::max();

        virtual ~IAudioBackend() = default;

        /// @brief Initialize the output device.
        /// @return `true` when the backend can create voices; otherwise `false` without throwing.
        [[nodiscard]] virtual bool initialize() noexcept = 0;
        /// @brief Stop and release every backend resource. Safe to call repeatedly.
        virtual void shutdown() noexcept = 0;

        /// @brief Create, configure, and start an independent voice transactionally.
        /// @param clip Immutable PCM data to play.
        /// @param params Initial gain and loop configuration.
        /// @return A backend voice ID, or InvalidVoice without publishing a partial resource.
        [[nodiscard("The returned voice must be managed and eventually destroyed")]]
        virtual VoiceId createAndStartVoice(const AudioClipData& clip,
                                            const PlayParams& params) noexcept = 0;
        /// @brief Release a voice and its native resources.
        /// @param voice Backend voice to release. Invalid IDs are ignored.
        virtual void destroyVoice(VoiceId voice) noexcept = 0;

        /// @brief Pause consumption without seeking from the current cursor.
        /// @param voice Backend voice to pause.
        /// @return `true` if the command was accepted.
        virtual bool pause(VoiceId voice) noexcept = 0;
        /// @brief Continue a paused voice from its retained cursor.
        /// @param voice Backend voice to resume.
        /// @return `true` if the command was accepted.
        virtual bool resume(VoiceId voice) noexcept = 0;
        /// @brief Halt a voice. AudioSystem subsequently destroys it.
        /// @param voice Backend voice to stop.
        /// @return `true` if the command was accepted.
        virtual bool stop(VoiceId voice) noexcept = 0;

        /// @brief Update a live voice's gain.
        /// @param voice Backend voice to update.
        /// @param volume New linear gain.
        /// @return `true` if the command was accepted.
        virtual bool setVolume(VoiceId voice, float volume) noexcept = 0;
        /// @brief Update whether a live voice wraps at the end of its clip.
        /// @param voice Backend voice to update.
        /// @param looping New looping policy.
        /// @return `true` if the command was accepted.
        virtual bool setLooping(VoiceId voice, bool looping) noexcept = 0;

        /// @brief Check whether a non-looping voice has naturally reached its end.
        /// @param voice Backend voice to query.
        /// @return `true` when playback has completed naturally.
        [[nodiscard]] virtual bool isAtEnd(VoiceId voice) const noexcept = 0;
    };
} // namespace Core::Audio
