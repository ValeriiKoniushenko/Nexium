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

namespace Core::Audio
{
    struct MiniaudioBackend::Impl final
    {
        ma_engine engine{};
        bool engineInitialized = false;

        Impl() = default;
        Impl(const Impl&) = delete;
        Impl& operator=(const Impl&) = delete;
        Impl(Impl&&) = delete;
        Impl& operator=(Impl&&) = delete;
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
    } // namespace

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
        if (_impl->engineInitialized)
        {
            return true;
        }

        const auto result = ma_engine_init(nullptr, &_impl->engine);
        if (result != MA_SUCCESS)
        {
            logInitializationFailure(result);
            return false;
        }

        _impl->engineInitialized = true;
        return true;
    }

    void MiniaudioBackend::shutdown() noexcept
    {
        if (!_impl->engineInitialized)
        {
            return;
        }

        ma_engine_uninit(&_impl->engine);
        _impl->engineInitialized = false;
    }

    IAudioBackend::VoiceId MiniaudioBackend::play(const AudioClipData&,
                                                  const PlayParams&) noexcept
    {
        return InvalidVoice;
    }

    void MiniaudioBackend::destroyVoice(VoiceId) noexcept
    {
    }

    bool MiniaudioBackend::pause(VoiceId) noexcept
    {
        return false;
    }

    bool MiniaudioBackend::resume(VoiceId) noexcept
    {
        return false;
    }

    bool MiniaudioBackend::stop(VoiceId) noexcept
    {
        return false;
    }

    bool MiniaudioBackend::setVolume(VoiceId, float) noexcept
    {
        return false;
    }

    bool MiniaudioBackend::setLooping(VoiceId, bool) noexcept
    {
        return false;
    }

    bool MiniaudioBackend::isAtEnd(VoiceId) const noexcept
    {
        return false;
    }
} // namespace Core::Audio
