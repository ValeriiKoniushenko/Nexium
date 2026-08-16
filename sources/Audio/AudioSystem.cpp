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

#include "Audio/AudioSystem.h"

#include "Audio/ModuleInfo.h"

#include <limits>
#include <utility>

namespace Core::Audio
{

    AudioSystem::AudioSystem(std::unique_ptr<IAudioBackend> backend)
        : _backend(std::move(backend))
    {
    }

    AudioSystem::~AudioSystem()
    {
        shutdown();
    }

    bool AudioSystem::initialize()
    {
        if (_initialized)
            return true;

        if (!_backend)
        {
            errorLog("Cannot initialize the audio system without a backend.");
            return false;
        }

        if (!_backend->initialize())
        {
            errorLog("Audio backend initialization failed. Audio will be unavailable.");
            return false;
        }

        _initialized = true;
        return true;
    }

    void AudioSystem::shutdown()
    {
        if (!_backend)
            return;

        stopAll();
        _backend->shutdown();
        _initialized = false;
    }

    void AudioSystem::update()
    {
        if (!isAvailable())
            return;

        for (std::uint32_t index = 0; index < _voices.size(); ++index)
        {
            const auto& slot = _voices[index];
            if (!slot.occupied || slot.state != PlaybackState::Playing)
                continue;

            if (_backend->isAtEnd(slot.backendVoice))
                retire(index);
        }
    }

    VoiceHandle AudioSystem::play(NXAudioClip clip, const PlayParams& params)
    {
        if (!isAvailable())
        {
            errorLog("Cannot play an audio clip while the audio system is unavailable.");
            return {};
        }

        if (!clip || !clip->isReady())
        {
            errorLog("Cannot play an invalid or unloaded audio clip.");
            return {};
        }

        const auto index = allocateSlot();
        if (index == VoiceHandle::InvalidIndex)
        {
            errorLog("Cannot allocate another audio voice slot.");
            return {};
        }

        auto& slot = _voices[index];
        slot.clip = std::move(clip);

        const auto backendVoice = _backend->createAndStartVoice(slot.clip->getData(), params);
        if (backendVoice == IAudioBackend::InvalidVoice)
        {
            releaseUnpublishedSlot(index);
            errorLog("The audio backend could not create a playback voice.");
            return {};
        }

        slot.backendVoice = backendVoice;
        slot.state = PlaybackState::Playing;
        return {index, slot.generation};
    }

    bool AudioSystem::pause(const VoiceHandle voice)
    {
        auto* slot = resolve(voice);
        if (!slot || slot->state != PlaybackState::Playing)
            return false;

        if (!_backend->pause(slot->backendVoice))
            return false;

        slot->state = PlaybackState::Paused;
        return true;
    }

    bool AudioSystem::resume(const VoiceHandle voice)
    {
        auto* slot = resolve(voice);
        if (!slot || slot->state != PlaybackState::Paused)
            return false;

        if (!_backend->resume(slot->backendVoice))
            return false;

        slot->state = PlaybackState::Playing;
        return true;
    }

    bool AudioSystem::stop(const VoiceHandle voice)
    {
        auto* slot = resolve(voice);
        if (!slot)
            return false;

        const auto index = voice.index;
        const bool stopped = _backend->stop(slot->backendVoice);
        retire(index);
        return stopped;
    }

    bool AudioSystem::setVolume(const VoiceHandle voice, const float volume)
    {
        const auto* slot = resolve(voice);
        return slot && _backend->setVolume(slot->backendVoice, volume);
    }

    bool AudioSystem::setLooping(const VoiceHandle voice, const bool loop)
    {
        const auto* slot = resolve(voice);
        return slot && _backend->setLooping(slot->backendVoice, loop);
    }

    PlaybackState AudioSystem::getState(const VoiceHandle voice) const
    {
        const auto* slot = resolve(voice);
        return slot ? slot->state : PlaybackState::Stopped;
    }

    bool AudioSystem::isValid(const VoiceHandle voice) const
    {
        return resolve(voice) != nullptr;
    }

    bool AudioSystem::isAvailable() const noexcept
    {
        return _initialized && _backend != nullptr;
    }

    void AudioSystem::stopAll()
    {
        if (!_backend)
            return;

        for (std::uint32_t index = 0; index < _voices.size(); ++index)
        {
            const auto& slot = _voices[index];
            if (!slot.occupied)
                continue;

            if (slot.backendVoice != IAudioBackend::InvalidVoice)
                _backend->stop(slot.backendVoice);

            retire(index);
        }
    }

    spdlog::logger* AudioSystem::getLogger() const
    {
        return ::Audio::getLogger();
    }

    AudioSystem::VoiceSlot* AudioSystem::resolve(const VoiceHandle voice) noexcept
    {
        if (!voice.isValid() || voice.index >= _voices.size())
            return nullptr;

        auto& slot = _voices[voice.index];
        if (!slot.occupied || slot.generation != voice.generation ||
            slot.backendVoice == IAudioBackend::InvalidVoice)
        {
            return nullptr;
        }

        return &slot;
    }

    const AudioSystem::VoiceSlot* AudioSystem::resolve(const VoiceHandle voice) const noexcept
    {
        if (!voice.isValid() || voice.index >= _voices.size())
            return nullptr;

        const auto& slot = _voices[voice.index];
        if (!slot.occupied || slot.generation != voice.generation ||
            slot.backendVoice == IAudioBackend::InvalidVoice)
        {
            return nullptr;
        }

        return &slot;
    }

    std::uint32_t AudioSystem::allocateSlot()
    {
        if (_freeVoiceHead != VoiceHandle::InvalidIndex)
        {
            const auto index = _freeVoiceHead;
            auto& slot = _voices[index];
            _freeVoiceHead = slot.nextFree;
            slot.nextFree = VoiceHandle::InvalidIndex;
            slot.occupied = true;
            return index;
        }

        if (_voices.size() >= VoiceHandle::InvalidIndex)
            return VoiceHandle::InvalidIndex;

        try
        {
            _voices.emplace_back();
        }
        catch (...)
        {
            return VoiceHandle::InvalidIndex;
        }

        const auto index = static_cast<std::uint32_t>(_voices.size() - 1);
        _voices[index].occupied = true;
        return index;
    }

    void AudioSystem::releaseUnpublishedSlot(const std::uint32_t index) noexcept
    {
        auto& slot = _voices[index];
        slot.clip = {};
        slot.backendVoice = IAudioBackend::InvalidVoice;
        slot.state = PlaybackState::Stopped;
        slot.occupied = false;
        slot.nextFree = _freeVoiceHead;
        _freeVoiceHead = index;
    }

    void AudioSystem::retire(const std::uint32_t index) noexcept
    {
        auto& slot = _voices[index];
        if (!slot.occupied)
            return;

        if (_backend && slot.backendVoice != IAudioBackend::InvalidVoice)
            _backend->destroyVoice(slot.backendVoice);

        slot.backendVoice = IAudioBackend::InvalidVoice;
        slot.state = PlaybackState::Stopped;
        slot.clip = {};
        slot.occupied = false;
        slot.nextFree = VoiceHandle::InvalidIndex;

        if (slot.generation == std::numeric_limits<std::uint32_t>::max())
            return;

        ++slot.generation;
        slot.nextFree = _freeVoiceHead;
        _freeVoiceHead = index;
    }

} // namespace Core::Audio
