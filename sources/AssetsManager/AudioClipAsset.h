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

#include "Audio/AudioTypes.h"
#include "BaseAsset.h"
#include "ResourceManagement/JsonAdapter.h"

namespace Core
{
    CLASS();
    class AudioClipAsset final : public BaseAsset
    {
        R_FRIEND_DECL(AudioClipAsset, Core::BaseAsset);

    public:
        inline static constexpr const char* fileExtension = ".nxaudio";

    public:
        explicit AudioClipAsset(const StringAtom& logicPath)
            : BaseAsset(logicPath)
        {
        }

        ~AudioClipAsset() override = default;

        [[nodiscard]] const char* getPrefix() const override { return "AudioClip"; }

        void setFilePath(const std::filesystem::path& value);
        [[nodiscard]] const std::filesystem::path& getFilePath() const noexcept { return _path; }

        [[nodiscard]] const Audio::AudioClipData& getData() const noexcept { return _data; }
        [[nodiscard]] bool isReady() const noexcept { return _data.isValid(); }

    protected:
        void onLoadRequest() override;
        void onUnloadRequest() override;
        void ioFieldsUpdate(DataStream& out) override;

    private:
        FIELD();
        std::filesystem::path _path;

        Audio::AudioClipData _data;
    };

    using NXAudioClip = AssetRef<AudioClipAsset>;
} // namespace Core

#include "AudioClipAsset.generated.h" // added by the code generator. Better don't move it.
