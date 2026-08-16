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

#include "IAudioBackend.h"

#include <memory>

namespace Core::Audio
{
    class MiniaudioBackend final : public IAudioBackend
    {
    public:
        MiniaudioBackend();
        ~MiniaudioBackend() override;

        MiniaudioBackend(const MiniaudioBackend&) = delete;
        MiniaudioBackend& operator=(const MiniaudioBackend&) = delete;
        MiniaudioBackend(MiniaudioBackend&&) = delete;
        MiniaudioBackend& operator=(MiniaudioBackend&&) = delete;

        [[nodiscard]] bool initialize() noexcept override;
        void shutdown() noexcept override;

        [[nodiscard("The returned voice must be managed and eventually destroyed")]]
        VoiceId createAndStartVoice(const AudioClipData& clip,
                                    const PlayParams& params) noexcept override;
        void destroyVoice(VoiceId voice) noexcept override;

        bool pause(VoiceId voice) noexcept override;
        bool resume(VoiceId voice) noexcept override;
        bool stop(VoiceId voice) noexcept override;

        bool setVolume(VoiceId voice, float volume) noexcept override;
        bool setLooping(VoiceId voice, bool looping) noexcept override;

        [[nodiscard]] bool isAtEnd(VoiceId voice) const noexcept override;

    private:
        struct Impl;
        std::unique_ptr<Impl> _impl;
    };
} // namespace Core::Audio
