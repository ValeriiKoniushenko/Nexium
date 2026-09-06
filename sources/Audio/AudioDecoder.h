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
#include <expected>
#include <filesystem>
#include <string_view>

namespace Core::Audio
{
    /// @brief Failure categories returned by the synchronous short-effect decoder.
    enum class AudioDecodeError : std::uint8_t
    {
        EmptyPath,
        FileNotFound,
        FileNotReadable,
        UnsupportedFormat,
        CorruptData,
        InvalidMetadata,
        NoAudioFrames,
        TooLarge,
        OutOfMemory,
        DecoderInitializationFailed,
        LengthQueryFailed,
        ReadFailed
    };

    /// @brief Result of decoding an audio file to an owned interleaved float PCM buffer.
    using AudioDecodeResult = std::expected<AudioClipData, AudioDecodeError>;

    /// @brief Decode a WAV, FLAC, or MP3 file into interleaved float PCM.
    ///
    /// This operation is synchronous and does not log; the caller owns path context and reporting.
    /// Successful data always has non-zero channels, sample rate, and frame count.
    /// @param path Source file path.
    /// @return Decoded PCM on success or a structured AudioDecodeError on failure.
    [[nodiscard]] AudioDecodeResult decodeAudioFile(const std::filesystem::path& path) noexcept;

    /// @brief Get a stable human-readable description for an AudioDecodeError.
    /// @param error Decoder error to describe.
    /// @return Non-owning description with static lifetime.
    [[nodiscard]] std::string_view getAudioDecodeErrorDescription(
        AudioDecodeError error) noexcept;
} // namespace Core::Audio
