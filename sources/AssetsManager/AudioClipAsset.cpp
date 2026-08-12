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

#include "AudioClipAsset.h"

#include "Audio/AudioDecoder.h"
#include "Misc/Configs.h"

#include <utility>

namespace Core
{
    R_FRIEND_IMPL(AudioClipAsset);

    void AudioClipAsset::setFilePath(const std::filesystem::path& value)
    {
        Assert(value.is_relative(), "Path should be relative");
        _path = value;
    }

    void AudioClipAsset::onLoadRequest()
    {
        _data = {};

        if (_path.empty()) [[unlikely]]
        {
            errorLog("Can't load audio clip '{}': the source path is empty."_f
                     << getLogicPath());
            return;
        }
        if (_path.is_absolute()) [[unlikely]]
        {
            errorLog("Can't load audio clip '{}': the source path '{}' must be relative."_f
                     << getLogicPath() << _path.generic_string());
            return;
        }

        const auto sourcePath = Config::Path::projectAbsPath / _path;
        auto decoded = Audio::decodeAudioFile(sourcePath);
        if (!decoded)
        {
            errorLog("Can't load audio clip '{}' from '{}': {}"_f
                     << getLogicPath() << _path.generic_string()
                     << Audio::getAudioDecodeErrorDescription(decoded.error()));
            return;
        }

        _data = std::move(*decoded);
    }

    void AudioClipAsset::onUnloadRequest()
    {
        _data = {};
    }
} // namespace Core
