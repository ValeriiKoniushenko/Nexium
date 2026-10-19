// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

// Module: Nexium::Subsystem::AssetsManager

#pragma once

#include "NxFundamental/Assets/BaseAsset.h"
#include "NxFundamental/ECS/BaseComponent.h"
#include "NxFundamental/ResourceManagement/JsonAdapter.h"
#include "NxSubsystems/Graphics/Font.h"
#include "RawBackend/Graphics/Texture.h"

namespace NX
{

    CLASS();
    class FontFaceAsset : public BaseAsset
    {
        R_FRIEND_DECL(FontFaceAsset, NX::BaseAsset);

    public:
        inline static const char* fileExtension = ".nxfface";

    public:
        explicit FontFaceAsset(const Core::StringAtom& logicPath)
            : BaseAsset(logicPath)
        {
        }
        ~FontFaceAsset() override = default;
        FontFaceAsset(const FontFaceAsset&) = delete;
        FontFaceAsset(FontFaceAsset&&) noexcept = delete;
        FontFaceAsset& operator=(const FontFaceAsset&) = delete;
        FontFaceAsset& operator=(FontFaceAsset&&) noexcept = delete;

        void setFilePath(const std::filesystem::path& value);
        [[nodiscard]] const std::filesystem::path& getFilePath() const noexcept { return _path; }

        [[nodiscard]] const NX::Face& getData() const noexcept { return _data; }
        [[nodiscard]] NX::Face& getData() noexcept { return _data; }

        // Overrides
        [[nodiscard]] const char* getPrefix() const override { return "FFace"; }

    protected:
        void onLoadRequest() override;
        void onUnloadRequest() override;

    protected:
        NX::Face _data;

        // properties

        FIELD();
        std::filesystem::path _path;
    };

    using NXFFace = AssetRef<FontFaceAsset>;
} // namespace NX

#include "FontFaceAsset.generated.h" // added by the code generator. Better don't move it.
