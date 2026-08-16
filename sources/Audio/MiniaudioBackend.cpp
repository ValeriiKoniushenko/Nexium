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

#include "MiniaudioBackend.h"

#include "ModuleInfo.h"
#include "miniaudio.h"
#include "spdlog/spdlog.h"

#include <memory>
#include <vector>

namespace Core::Audio
{
    struct MiniaudioBackend::Impl final
    {
    public:
        Impl() = default;
        Impl(const Impl&) = delete;
        Impl& operator=(const Impl&) = delete;
        Impl(Impl&&) = delete;
        Impl& operator=(Impl&&) = delete;

        [[nodiscard]] bool initialize() noexcept;
        void shutdown() noexcept;

        [[nodiscard("The returned voice must be managed and eventually destroyed")]]
        VoiceId createAndStartVoice(const AudioClipData& clip,
                                    const PlayParams& params) noexcept;
        void destroyVoice(VoiceId voice) noexcept;

        bool pause(VoiceId voice) noexcept;
        bool resume(VoiceId voice) noexcept;
        bool stop(VoiceId voice) noexcept;
        bool setVolume(VoiceId voice, float volume) noexcept;
        bool setLooping(VoiceId voice, bool looping) noexcept;
        [[nodiscard]] bool isAtEnd(VoiceId voice) const noexcept;

    private:
        struct Voice final
        {
            ma_audio_buffer dataSource{};
            ma_sound sound{};
            bool dataSourceInitialized = false;
            bool soundInitialized = false;

            Voice() = default;
            Voice(const Voice&) = delete;
            Voice& operator=(const Voice&) = delete;
            Voice(Voice&&) = delete;
            Voice& operator=(Voice&&) = delete;
            ~Voice();
        };

        struct VoiceSlot final
        {
            std::unique_ptr<Voice> voice;
            VoiceId nextFree = InvalidVoice;
        };

        [[nodiscard]] Voice* findVoice(VoiceId voice) noexcept;
        [[nodiscard]] const Voice* findVoice(VoiceId voice) const noexcept;
        [[nodiscard]] VoiceId acquireVoiceSlot() noexcept;
        void releaseVoiceSlot(VoiceId voice) noexcept;
        [[nodiscard]] bool haltVoice(VoiceId voice) noexcept;

        ma_engine engine{};
        bool engineInitialized = false;
        std::vector<VoiceSlot> voiceSlots;
        VoiceId freeVoiceHead = InvalidVoice;
    };

    namespace
    {
        void logInitializationFailure(ma_result result) noexcept
        {
            try
            {
                ::Audio::getLogger()->error("Failed to initialize miniaudio backend: {} ({})",
                                            ma_result_description(result), static_cast<int>(result));
            }
            catch (...)
            {
                // Logging must not turn an unavailable audio device into a fatal startup error.
            }
        }

        void logPlaybackFailure(const char* operation, ma_result result) noexcept
        {
            try
            {
                ::Audio::getLogger()->error("Failed to {} miniaudio voice: {} ({})", operation,
                                            ma_result_description(result),
                                            static_cast<int>(result));
            }
            catch (...)
            {
                // Audio failure reporting must not escape a noexcept backend command.
            }
        }
    } // namespace

    MiniaudioBackend::Impl::Voice::~Voice()
    {
        if (soundInitialized)
        {
            ma_sound_uninit(&sound);
        }
        if (dataSourceInitialized)
        {
            ma_audio_buffer_uninit(&dataSource);
        }
    }

    bool MiniaudioBackend::Impl::initialize() noexcept
    {
        if (engineInitialized)
        {
            return true;
        }

        const auto result = ma_engine_init(nullptr, &engine);
        if (result != MA_SUCCESS)
        {
            logInitializationFailure(result);
            return false;
        }

        engineInitialized = true;
        return true;
    }

    void MiniaudioBackend::Impl::shutdown() noexcept
    {
        if (!engineInitialized)
        {
            return;
        }

        voiceSlots.clear();
        freeVoiceHead = InvalidVoice;
        ma_engine_uninit(&engine);
        engineInitialized = false;
    }

    IAudioBackend::VoiceId MiniaudioBackend::Impl::acquireVoiceSlot() noexcept
    {
        if (freeVoiceHead != InvalidVoice)
        {
            const auto voice = freeVoiceHead;
            auto& slot = voiceSlots[voice];
            freeVoiceHead = slot.nextFree;
            slot.nextFree = InvalidVoice;
            return voice;
        }

        if (voiceSlots.size() >= InvalidVoice)
        {
            return InvalidVoice;
        }

        try
        {
            const auto voice = static_cast<VoiceId>(voiceSlots.size());
            voiceSlots.emplace_back();
            return voice;
        }
        catch (...)
        {
            return InvalidVoice;
        }
    }

    void MiniaudioBackend::Impl::releaseVoiceSlot(VoiceId voice) noexcept
    {
        auto& slot = voiceSlots[voice];
        slot.nextFree = freeVoiceHead;
        freeVoiceHead = voice;
    }

    MiniaudioBackend::Impl::Voice* MiniaudioBackend::Impl::findVoice(VoiceId voice) noexcept
    {
        if (voice >= voiceSlots.size())
        {
            return nullptr;
        }

        return voiceSlots[voice].voice.get();
    }

    const MiniaudioBackend::Impl::Voice* MiniaudioBackend::Impl::findVoice(
        VoiceId voice) const noexcept
    {
        if (voice >= voiceSlots.size())
        {
            return nullptr;
        }

        return voiceSlots[voice].voice.get();
    }

    IAudioBackend::VoiceId MiniaudioBackend::Impl::createAndStartVoice(
        const AudioClipData& clip, const PlayParams& params) noexcept
    {
        if (!engineInitialized || !clip.isValid())
        {
            return InvalidVoice;
        }

        const auto voiceId = acquireVoiceSlot();
        if (voiceId == InvalidVoice)
        {
            return InvalidVoice;
        }

        try
        {
            auto voice = std::make_unique<Voice>();
            auto dataSourceConfig =
                ma_audio_buffer_config_init(ma_format_f32, clip.channels, clip.frameCount(),
                                            clip.interleavedSamples.data(), nullptr);
            dataSourceConfig.sampleRate = clip.sampleRate;

            auto result = ma_audio_buffer_init(&dataSourceConfig, &voice->dataSource);
            if (result != MA_SUCCESS)
            {
                logPlaybackFailure("initialize data source for", result);
                releaseVoiceSlot(voiceId);
                return InvalidVoice;
            }
            voice->dataSourceInitialized = true;

            result = ma_sound_init_from_data_source(&engine, &voice->dataSource,
                                                    MA_SOUND_FLAG_NO_SPATIALIZATION, nullptr,
                                                    &voice->sound);
            if (result != MA_SUCCESS)
            {
                logPlaybackFailure("initialize", result);
                releaseVoiceSlot(voiceId);
                return InvalidVoice;
            }
            voice->soundInitialized = true;

            ma_sound_set_volume(&voice->sound, params.volume);
            ma_sound_set_looping(&voice->sound, params.loop ? MA_TRUE : MA_FALSE);

            result = ma_sound_start(&voice->sound);
            if (result != MA_SUCCESS)
            {
                logPlaybackFailure("start", result);
                releaseVoiceSlot(voiceId);
                return InvalidVoice;
            }

            voiceSlots[voiceId].voice = std::move(voice);
            return voiceId;
        }
        catch (...)
        {
            releaseVoiceSlot(voiceId);
            return InvalidVoice;
        }
    }

    void MiniaudioBackend::Impl::destroyVoice(VoiceId voice) noexcept
    {
        if (findVoice(voice) == nullptr)
        {
            return;
        }

        voiceSlots[voice].voice.reset();
        releaseVoiceSlot(voice);
    }

    bool MiniaudioBackend::Impl::haltVoice(VoiceId voice) noexcept
    {
        auto* nativeVoice = findVoice(voice);
        return nativeVoice != nullptr && ma_sound_stop(&nativeVoice->sound) == MA_SUCCESS;
    }

    bool MiniaudioBackend::Impl::pause(VoiceId voice) noexcept
    {
        return haltVoice(voice);
    }

    bool MiniaudioBackend::Impl::resume(VoiceId voice) noexcept
    {
        auto* nativeVoice = findVoice(voice);
        return nativeVoice != nullptr && ma_sound_start(&nativeVoice->sound) == MA_SUCCESS;
    }

    bool MiniaudioBackend::Impl::stop(VoiceId voice) noexcept
    {
        return haltVoice(voice);
    }

    bool MiniaudioBackend::Impl::setVolume(VoiceId voice, float volume) noexcept
    {
        auto* nativeVoice = findVoice(voice);
        if (nativeVoice == nullptr)
        {
            return false;
        }

        ma_sound_set_volume(&nativeVoice->sound, volume);
        return true;
    }

    bool MiniaudioBackend::Impl::setLooping(VoiceId voice, bool looping) noexcept
    {
        auto* nativeVoice = findVoice(voice);
        if (nativeVoice == nullptr)
        {
            return false;
        }

        ma_sound_set_looping(&nativeVoice->sound, looping ? MA_TRUE : MA_FALSE);
        return true;
    }

    bool MiniaudioBackend::Impl::isAtEnd(VoiceId voice) const noexcept
    {
        const auto* nativeVoice = findVoice(voice);
        return nativeVoice != nullptr && ma_sound_at_end(&nativeVoice->sound) == MA_TRUE;
    }

    MiniaudioBackend::MiniaudioBackend()
        : _impl(std::make_unique<Impl>())
    {
    }

    MiniaudioBackend::~MiniaudioBackend()
    {
        shutdown();
    }

    bool MiniaudioBackend::initialize() noexcept
    {
        return _impl->initialize();
    }

    void MiniaudioBackend::shutdown() noexcept
    {
        _impl->shutdown();
    }

    IAudioBackend::VoiceId MiniaudioBackend::createAndStartVoice(
        const AudioClipData& clip, const PlayParams& params) noexcept
    {
        return _impl->createAndStartVoice(clip, params);
    }

    void MiniaudioBackend::destroyVoice(VoiceId voice) noexcept
    {
        _impl->destroyVoice(voice);
    }

    bool MiniaudioBackend::pause(VoiceId voice) noexcept
    {
        return _impl->pause(voice);
    }

    bool MiniaudioBackend::resume(VoiceId voice) noexcept
    {
        return _impl->resume(voice);
    }

    bool MiniaudioBackend::stop(VoiceId voice) noexcept
    {
        return _impl->stop(voice);
    }

    bool MiniaudioBackend::setVolume(VoiceId voice, float volume) noexcept
    {
        return _impl->setVolume(voice, volume);
    }

    bool MiniaudioBackend::setLooping(VoiceId voice, bool looping) noexcept
    {
        return _impl->setLooping(voice, looping);
    }

    bool MiniaudioBackend::isAtEnd(VoiceId voice) const noexcept
    {
        const auto& impl = *_impl;
        return impl.isAtEnd(voice);
    }
} // namespace Core::Audio
