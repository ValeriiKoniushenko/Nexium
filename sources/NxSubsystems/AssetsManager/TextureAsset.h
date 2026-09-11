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

#include "NxFundamental/Assets/BaseAsset.h"
#include "NxFundamental/ECS/BaseComponent.h"
#include "NxFundamental/ResourceManagement/JsonAdapter.h"
#include "RawBackend/Graphics/Texture.h"

namespace NX
{

    CLASS();
    class TextureAsset : public BaseAsset
    {
        R_FRIEND_DECL(TextureAsset, NX::BaseAsset);

    public:
        inline static const char* fileExtension = ".nxtex";

    public:
        explicit TextureAsset(const Core::StringAtom& logicPath)
            : BaseAsset(logicPath)
        {
        }

        ~TextureAsset() override = default;
        TextureAsset(const TextureAsset&) = delete;
        TextureAsset(TextureAsset&&) noexcept = delete;
        TextureAsset& operator=(const TextureAsset&) = delete;
        TextureAsset& operator=(TextureAsset&&) noexcept = delete;

        [[nodiscard]] const RawBackend::Texture& getData() const noexcept { return _data; }
        [[nodiscard]] RawBackend::Texture& getData() noexcept { return _data; }

        [[nodiscard]] const char* getPrefix() const override { return "Texture"; }

        void setFilePath(const std::filesystem::path& value);
        [[nodiscard]] const std::filesystem::path& getFilePath() const noexcept { return _path; }

        void setFlipVertically(bool value) noexcept { _isFlipVertically = value; }
        [[nodiscard]] bool isFlipVertically() const noexcept { return _isFlipVertically; }

        [[nodiscard]] RawBackend::Image::Channel getChannels() const noexcept { return _channels; }
        [[nodiscard]] Core::ISize2 getSize() const noexcept { return _size; }

    protected:
        void onLoadRequest() override;
        void onUnloadRequest() override;

    protected:
        RawBackend::Texture _data;

        // properties

        FIELD();
        std::filesystem::path _path;

        RawBackend::Image::Channel _channels = RawBackend::Image::Channel::None;
        Core::ISize2 _size = {};

        FIELD();
        bool _isFlipVertically = false;
    };

    using NXTexture = AssetRef<TextureAsset>;
} // namespace NX

#include "TextureAsset.generated.h" // added by the code generator. Better don't move it.
