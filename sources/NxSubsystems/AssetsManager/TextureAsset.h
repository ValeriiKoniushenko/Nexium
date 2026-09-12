// Nexium
// Copyright 2018-2027 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

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
