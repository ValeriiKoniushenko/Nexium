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

#include "Audio/AudioSource.h"

#include "GameplaySystem/Framework/GameInstance.h"

#include <algorithm>
#include <cmath>

namespace Core::Audio
{
    ECS_IMPL(AudioSource);

    AudioSource::AudioSource(const StringAtom& name)
        : BaseComponent(componentType, name)
    {
        setNoTick(true);
    }

    AudioSource::AudioSource(const AudioSource& other)
        : BaseComponent(other),
          _clipPath(other._clipPath),
          _volume(other._volume),
          _loop(other._loop),
          _maxPolyphony(other._maxPolyphony)
    {
        setNoTick(true);
    }

    AudioSource::AudioSource(AudioSource&& other) noexcept
        : BaseComponent(std::move(other)),
          _clipPath(std::move(other._clipPath)),
          _volume(other._volume),
          _loop(other._loop),
          _maxPolyphony(other._maxPolyphony),
          _clip(std::move(other._clip)),
          _voices(std::move(other._voices))
    {
        setNoTick(true);
        other._clip = {};
        other._voices.clear();
        other.setNoTick(true);
    }

    AudioSource& AudioSource::operator=(const AudioSource& other)
    {
        if (this == &other)
            return *this;

        const bool refreshRuntimeState = isInitialized();
        stop();
        _clip = {};

        BaseComponent::operator=(other);
        _clipPath = other._clipPath;
        _volume = other._volume;
        _loop = other._loop;
        _maxPolyphony = other._maxPolyphony;
        normalizeConfiguration();

        if (refreshRuntimeState)
        {
            if (isInitialized())
            {
                _voices.reserve(_maxPolyphony);
                resolveConfiguredClip();
            }
            else
            {
                initialize();
            }
        }
        else
            invalidate();

        return *this;
    }

    AudioSource& AudioSource::operator=(AudioSource&& other) noexcept
    {
        if (this == &other)
            return *this;

        const bool sourceWasInitialized = other.isInitialized();
        stop();
        _clip = {};

        BaseComponent::operator=(std::move(other));
        _clipPath = std::move(other._clipPath);
        _volume = other._volume;
        _loop = other._loop;
        _maxPolyphony = other._maxPolyphony;
        _clip = std::move(other._clip);
        _voices = std::move(other._voices);

        setNoTick(true);
        other._clip = {};
        other._voices.clear();
        other.setNoTick(true);

        if (sourceWasInitialized && !isInitialized())
            initialize();
        else if (!sourceWasInitialized)
            invalidate();

        return *this;
    }

    AudioSource::~AudioSource()
    {
        if (!_voices.empty())
            stop();
    }

    VoiceHandle AudioSource::play()
    {
        pruneInvalidVoices();
        if (!_clip || !_clip->isReady())
            return {};

        while (_voices.size() >= _maxPolyphony)
            stopOldestVoice();

        const auto voice = GetAudioSystem().play(
            _clip, PlayParams{ .volume = _volume, .loop = _loop });
        if (voice.isValid())
            _voices.push_back(voice);

        return voice;
    }

    bool AudioSource::pause()
    {
        pruneInvalidVoices();
        if (_voices.empty())
            return false;

        auto& audioSystem = GetAudioSystem();
        bool paused = false;
        for (const auto voice : _voices)
            paused = audioSystem.pause(voice) || paused;

        return paused;
    }

    bool AudioSource::resume()
    {
        pruneInvalidVoices();
        if (_voices.empty())
            return false;

        auto& audioSystem = GetAudioSystem();
        bool resumed = false;
        for (const auto voice : _voices)
            resumed = audioSystem.resume(voice) || resumed;

        return resumed;
    }

    bool AudioSource::stop()
    {
        pruneInvalidVoices();
        if (_voices.empty())
            return false;

        auto& audioSystem = GetAudioSystem();
        bool stopped = false;
        for (const auto voice : _voices)
            stopped = audioSystem.stop(voice) || stopped;

        _voices.clear();
        return stopped;
    }

    bool AudioSource::execute(const AudioCommand command)
    {
        switch (command)
        {
            case AudioCommand::Play:
                return play().isValid();
            case AudioCommand::Pause:
                return pause();
            case AudioCommand::Resume:
                return resume();
            case AudioCommand::Stop:
                return stop();
        }

        return false;
    }

    void AudioSource::setClip(const StringAtom& logicalPath)
    {
        auto normalizedPath
            = logicalPath.isEmpty() ? StringAtom{} : StringAtom::Intern(logicalPath);
        const bool changed = _clipPath != normalizedPath;
        if (!changed)
        {
            _clipPath = std::move(normalizedPath);
            return;
        }

        if (isInitialized())
            stop();

        _clip = {};
        _clipPath = std::move(normalizedPath);

        if (isInitialized())
            resolveConfiguredClip();
    }

    void AudioSource::setVolume(const float volume)
    {
        _volume = std::isnan(volume) ? maxVolume : std::clamp(volume, minVolume, maxVolume);

        pruneInvalidVoices();
        if (_voices.empty())
            return;

        auto& audioSystem = GetAudioSystem();
        for (const auto voice : _voices)
            audioSystem.setVolume(voice, _volume);
    }

    void AudioSource::setLooping(const bool loop)
    {
        if (loop)
            setMaxPolyphony(minPolyphony);

        _loop = loop;

        pruneInvalidVoices();
        if (_voices.empty())
            return;

        auto& audioSystem = GetAudioSystem();
        for (const auto voice : _voices)
            audioSystem.setLooping(voice, _loop);
    }

    void AudioSource::setMaxPolyphony(const std::uint32_t maxPolyphony)
    {
        const auto normalized = _loop
                                    ? minPolyphony
                                    : std::clamp(maxPolyphony, minPolyphony,
                                                 AudioSource::maxPolyphony);

        if (isInitialized() && normalized > _voices.capacity())
            _voices.reserve(normalized);

        _maxPolyphony = normalized;

        pruneInvalidVoices();
        while (_voices.size() > _maxPolyphony)
            stopOldestVoice();
    }

    PlaybackState AudioSource::getPlaybackState() const
    {
        pruneInvalidVoices();
        if (_voices.empty())
            return PlaybackState::Stopped;

        const auto& audioSystem = GetAudioSystem();
        auto state = PlaybackState::Stopped;
        for (const auto voice : _voices)
        {
            const auto voiceState = audioSystem.getState(voice);
            if (voiceState == PlaybackState::Playing)
                return PlaybackState::Playing;
            if (voiceState == PlaybackState::Paused)
                state = PlaybackState::Paused;
        }

        return state;
    }

    bool AudioSource::isPlaying() const
    {
        return getPlaybackState() == PlaybackState::Playing;
    }

    bool AudioSource::isPaused() const
    {
        return getPlaybackState() == PlaybackState::Paused;
    }

    bool AudioSource::isStopped() const
    {
        return getPlaybackState() == PlaybackState::Stopped;
    }

    Tag AudioSource::getTags() const
    {
        return BaseComponent::getTags() | Tag_AudioSource;
    }

    void AudioSource::onPostDeserialize(AbstractComponent* obj, const RLogsCollector& logs)
    {
        BaseComponent::onPostDeserialize(obj, logs);

        stop();
        _clip = {};
        invalidate();
        normalizeConfiguration();
    }

    void AudioSource::onInitialize()
    {
        normalizeConfiguration();
        _voices.reserve(_maxPolyphony);
        resolveConfiguredClip();
    }

    void AudioSource::normalizeConfiguration()
    {
        setNoTick(true);
        setClip(_clipPath);
        setVolume(_volume);
        setMaxPolyphony(_maxPolyphony);
    }

    void AudioSource::resolveConfiguredClip()
    {
        _clip = _clipPath.isEmpty() ? NXAudioClip{} : GetAssetsManager().getAudioClip(_clipPath);
    }

    void AudioSource::pruneInvalidVoices() const
    {
        if (_voices.empty())
            return;

        const auto& audioSystem = GetAudioSystem();
        std::erase_if(_voices,
                      [&audioSystem](const VoiceHandle voice)
                      { return !audioSystem.isValid(voice); });
    }

    void AudioSource::stopOldestVoice()
    {
        if (_voices.empty())
            return;

        GetAudioSystem().stop(_voices.front());
        _voices.erase(_voices.begin());
    }
} // namespace Core::Audio
