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
    class IAudioBackend
    {
    public:
        using VoiceId = std::uint32_t;
        static constexpr VoiceId InvalidVoice = std::numeric_limits<VoiceId>::max();

        virtual ~IAudioBackend() = default;

        [[nodiscard]] virtual bool initialize() noexcept = 0;
        virtual void shutdown() noexcept = 0;

        [[nodiscard("The returned voice must be managed and eventually destroyed")]]
        virtual VoiceId createAndStartVoice(const AudioClipData& clip,
                                            const PlayParams& params) noexcept = 0;
        virtual void destroyVoice(VoiceId voice) noexcept = 0;

        virtual bool pause(VoiceId voice) noexcept = 0;
        virtual bool resume(VoiceId voice) noexcept = 0;
        virtual bool stop(VoiceId voice) noexcept = 0;

        virtual bool setVolume(VoiceId voice, float volume) noexcept = 0;
        virtual bool setLooping(VoiceId voice, bool looping) noexcept = 0;

        [[nodiscard]] virtual bool isAtEnd(VoiceId voice) const noexcept = 0;
    };
} // namespace Core::Audio
