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

#include <compare>
#include <cstdint>
#include <limits>
#include <vector>

namespace Core::Audio
{
    enum class PlaybackState : std::uint8_t
    {
        Stopped,
        Playing,
        Paused
    };

    enum class AudioCommand : std::uint8_t
    {
        Play,
        Pause,
        Resume,
        Stop
    };

    struct VoiceHandle
    {
        static constexpr std::uint32_t InvalidIndex = std::numeric_limits<std::uint32_t>::max();

        std::uint32_t index = InvalidIndex;
        std::uint32_t generation = 0;

        [[nodiscard]] constexpr bool isValid() const noexcept
        {
            return index != InvalidIndex && generation != 0;
        }

        constexpr auto operator<=>(const VoiceHandle&) const = default;
    };

    struct PlayParams
    {
        float volume = 1.f;
        bool loop = false;
    };

    struct AudioClipData
    {
        std::vector<float> interleavedSamples;
        std::uint32_t channels = 0;
        std::uint32_t sampleRate = 0;

        [[nodiscard]] std::uint64_t frameCount() const noexcept
        {
            if (channels == 0 || interleavedSamples.size() % channels != 0)
            {
                return 0;
            }

            return static_cast<std::uint64_t>(interleavedSamples.size() / channels);
        }

        [[nodiscard]] bool isValid() const noexcept
        {
            return sampleRate != 0 && frameCount() != 0;
        }
    };
} // namespace Core::Audio
