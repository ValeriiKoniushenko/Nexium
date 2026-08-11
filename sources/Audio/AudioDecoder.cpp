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

#include "AudioDecoder.h"

#include "miniaudio.h"

#include <new>
#include <stdexcept>
#include <system_error>

namespace Core::Audio
{
    namespace
    {
        struct NativeDecoder final
        {
            ma_decoder value{};
            bool initialized = false;

            NativeDecoder() = default;
            NativeDecoder(const NativeDecoder&) = delete;
            NativeDecoder& operator=(const NativeDecoder&) = delete;
            NativeDecoder(NativeDecoder&&) = delete;
            NativeDecoder& operator=(NativeDecoder&&) = delete;

            ~NativeDecoder()
            {
                if (initialized)
                {
                    ma_decoder_uninit(&value);
                }
            }
        };

        [[nodiscard]] AudioDecodeError mapInitializationError(ma_result result) noexcept
        {
            switch (result)
            {
                case MA_DOES_NOT_EXIST:
                    return AudioDecodeError::FileNotFound;
                case MA_ACCESS_DENIED:
                case MA_TOO_MANY_OPEN_FILES:
                case MA_PATH_TOO_LONG:
                case MA_NAME_TOO_LONG:
                case MA_NOT_DIRECTORY:
                case MA_IS_DIRECTORY:
                case MA_IO_ERROR:
                    return AudioDecodeError::FileNotReadable;
                case MA_FORMAT_NOT_SUPPORTED:
                case MA_NO_BACKEND:
                case MA_NOT_IMPLEMENTED:
                    return AudioDecodeError::UnsupportedFormat;
                case MA_INVALID_FILE:
                case MA_INVALID_DATA:
                case MA_CRC_MISMATCH:
                    return AudioDecodeError::CorruptData;
                case MA_TOO_BIG:
                case MA_OUT_OF_RANGE:
                    return AudioDecodeError::TooLarge;
                case MA_OUT_OF_MEMORY:
                    return AudioDecodeError::OutOfMemory;
                default:
                    return AudioDecodeError::DecoderInitializationFailed;
            }
        }

        [[nodiscard]] AudioDecodeError mapProcessingError(
            ma_result result, AudioDecodeError fallback) noexcept
        {
            switch (result)
            {
                case MA_OUT_OF_MEMORY:
                    return AudioDecodeError::OutOfMemory;
                case MA_TOO_BIG:
                case MA_OUT_OF_RANGE:
                    return AudioDecodeError::TooLarge;
                case MA_INVALID_FILE:
                case MA_INVALID_DATA:
                case MA_CRC_MISMATCH:
                    return AudioDecodeError::CorruptData;
                default:
                    return fallback;
            }
        }

        [[nodiscard]] AudioDecodeResult fail(AudioDecodeError error) noexcept
        {
            return std::unexpected(error);
        }
    } // namespace

    AudioDecodeResult decodeAudioFile(const std::filesystem::path& path) noexcept
    {
        if (path.empty())
        {
            return fail(AudioDecodeError::EmptyPath);
        }

        std::error_code fileError;
        const auto fileStatus = std::filesystem::status(path, fileError);
        if (fileError)
        {
            if (fileError == std::errc::no_such_file_or_directory)
            {
                return fail(AudioDecodeError::FileNotFound);
            }

            return fail(AudioDecodeError::FileNotReadable);
        }
        if (!std::filesystem::exists(fileStatus))
        {
            return fail(AudioDecodeError::FileNotFound);
        }
        if (!std::filesystem::is_regular_file(fileStatus))
        {
            return fail(AudioDecodeError::FileNotReadable);
        }

        NativeDecoder decoder;
        const auto config = ma_decoder_config_init(ma_format_f32, 0, 0);

#if defined(_WIN32)
        const auto initializationResult
            = ma_decoder_init_file_w(path.c_str(), &config, &decoder.value);
#else
        const auto initializationResult
            = ma_decoder_init_file(path.c_str(), &config, &decoder.value);
#endif
        if (initializationResult != MA_SUCCESS)
        {
            return fail(mapInitializationError(initializationResult));
        }
        decoder.initialized = true;

        const auto channels = decoder.value.outputChannels;
        const auto sampleRate = decoder.value.outputSampleRate;
        if (channels == 0 || sampleRate == 0)
        {
            return fail(AudioDecodeError::InvalidMetadata);
        }

        ma_uint64 frameCount = 0;
        const auto lengthResult
            = ma_decoder_get_length_in_pcm_frames(&decoder.value, &frameCount);
        if (lengthResult != MA_SUCCESS)
        {
            return fail(mapProcessingError(lengthResult, AudioDecodeError::LengthQueryFailed));
        }
        if (frameCount == 0)
        {
            return fail(AudioDecodeError::NoAudioFrames);
        }

        AudioClipData data;
        const auto maxSampleCount = static_cast<ma_uint64>(data.interleavedSamples.max_size());
        if (frameCount > maxSampleCount / channels)
        {
            return fail(AudioDecodeError::TooLarge);
        }

        const auto sampleCount = static_cast<std::size_t>(frameCount * channels);
        try
        {
            data.interleavedSamples.resize(sampleCount);
        }
        catch (const std::bad_alloc&)
        {
            return fail(AudioDecodeError::OutOfMemory);
        }
        catch (const std::length_error&)
        {
            return fail(AudioDecodeError::TooLarge);
        }

        ma_uint64 framesRead = 0;
        const auto readResult = ma_decoder_read_pcm_frames(
            &decoder.value, data.interleavedSamples.data(), frameCount, &framesRead);
        if ((readResult != MA_SUCCESS && readResult != MA_AT_END) || framesRead != frameCount)
        {
            return fail(mapProcessingError(readResult, AudioDecodeError::ReadFailed));
        }

        data.channels = channels;
        data.sampleRate = sampleRate;
        if (!data.isValid() || data.frameCount() != frameCount)
        {
            return fail(AudioDecodeError::InvalidMetadata);
        }

        return data;
    }

    std::string_view getAudioDecodeErrorDescription(AudioDecodeError error) noexcept
    {
        switch (error)
        {
            case AudioDecodeError::EmptyPath:
                return "The audio file path is empty.";
            case AudioDecodeError::FileNotFound:
                return "The audio file does not exist.";
            case AudioDecodeError::FileNotReadable:
                return "The audio file cannot be read.";
            case AudioDecodeError::UnsupportedFormat:
                return "The audio format is not supported.";
            case AudioDecodeError::CorruptData:
                return "The audio file is corrupt or contains invalid data.";
            case AudioDecodeError::InvalidMetadata:
                return "The decoded audio metadata is invalid.";
            case AudioDecodeError::NoAudioFrames:
                return "The audio file contains no PCM frames.";
            case AudioDecodeError::TooLarge:
                return "The decoded audio data is too large.";
            case AudioDecodeError::OutOfMemory:
                return "There is not enough memory to decode the audio file.";
            case AudioDecodeError::DecoderInitializationFailed:
                return "The audio decoder could not be initialized.";
            case AudioDecodeError::LengthQueryFailed:
                return "The decoded audio length could not be determined.";
            case AudioDecodeError::ReadFailed:
                return "The audio PCM data could not be read completely.";
        }

        return "Unknown audio decoding error.";
    }
} // namespace Core::Audio
