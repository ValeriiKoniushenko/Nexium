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
#include "ECS/BaseComponent.h"

#include <cstdint>
#include <vector>

namespace Core::Audio
{
    /// @brief Serializable ECS component that configures and controls related sound-effect voices.
    ///
    /// The configured clip path, gain, loop flag, and polyphony are serialized. Runtime handles
    /// are not. Starting beyond the polyphony limit stops the oldest active voice first.
    CLASS();
    class AudioSource final : public BaseComponent
    {
        ECS_DECL_NO_CNSTR(AudioSource, Core::BaseComponent);

    public:
        /// @brief Inclusive configuration limits enforced by setters and deserialization.
        inline static constexpr float minVolume = 0.f;
        inline static constexpr float maxVolume = 1.f;
        inline static constexpr std::uint32_t minPolyphony = 1;
        inline static constexpr std::uint32_t maxPolyphony = 32;

        explicit AudioSource(const StringAtom& name = ""_atom);
        AudioSource(const AudioSource& other);
        AudioSource(AudioSource&& other) noexcept;
        AudioSource& operator=(const AudioSource& other);
        AudioSource& operator=(AudioSource&& other) noexcept;
        ~AudioSource() override;

        /// @brief Start a new voice for the configured ready clip.
        /// @return A valid handle on success; an invalid handle otherwise.
        VoiceHandle play();
        /// @brief Pause every active voice created by this source.
        /// @return `true` when at least one voice was paused.
        bool pause();
        /// @brief Resume every paused voice created by this source.
        /// @return `true` when at least one voice was resumed.
        bool resume();
        /// @brief Stop and release every active voice created by this source.
        /// @return `true` when at least one voice was stopped.
        bool stop();
        /// @brief Apply a command to all currently active voices of this source.
        /// @param command Playback command to execute.
        /// @return `true` when the command affects at least one voice.
        bool execute(AudioCommand command);

        /// @brief Set the logical `.nxaudio` path and resolve it when initialized.
        /// @param logicalPath Asset-manager logical path, or an empty path to clear the clip.
        void setClip(const StringAtom& logicalPath);
        /// @brief Set the initial gain for newly created voices.
        /// @param volume Requested linear gain, clamped to [minVolume, maxVolume].
        void setVolume(float volume);
        /// @brief Set whether newly created voices loop.
        /// @param loop `true` to wrap a clip at its end.
        void setLooping(bool loop);
        /// @brief Limit concurrently active voices.
        /// @param maxPolyphony Requested limit, clamped to [minPolyphony, maxPolyphony].
        void setMaxPolyphony(std::uint32_t maxPolyphony);

        [[nodiscard]] const StringAtom& getClipPath() const noexcept { return _clipPath; }
        [[nodiscard]] float getVolume() const noexcept { return _volume; }
        [[nodiscard]] bool isLooping() const noexcept { return _loop; }
        [[nodiscard]] std::uint32_t getMaxPolyphony() const noexcept { return _maxPolyphony; }

        /// @brief Get the aggregate state of voices owned by this source.
        /// @return Playing when any voice plays, Paused when none play but one is paused, otherwise
        /// Stopped.
        [[nodiscard]] PlaybackState getPlaybackState() const;
        [[nodiscard]] bool isPlaying() const;
        [[nodiscard]] bool isPaused() const;
        [[nodiscard]] bool isStopped() const;

        [[nodiscard]] Tag getTags() const override;
        void onPostDeserialize(AbstractComponent* obj, const RLogsCollector& logs) override;

    protected:
        void onInitialize() override;

    private:
        void normalizeConfiguration();
        void resolveConfiguredClip();
        void pruneInvalidVoices() const;
        void stopOldestVoice();

        FIELD();
        StringAtom _clipPath;

        FIELD();
        float _volume = 1.f;

        FIELD();
        bool _loop = false;

        FIELD();
        std::uint32_t _maxPolyphony = 1;

        NXAudioClip _clip;
        mutable std::vector<VoiceHandle> _voices;
    };
} // namespace Core::Audio

#include "AudioSource.generated.h" // added by the code generator. Better don't move it.
