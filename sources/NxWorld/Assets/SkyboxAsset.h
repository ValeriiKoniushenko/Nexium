// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "NxFundamental/Assets/BaseAsset.h"
#include "NxFundamental/ResourceManagement/JsonAdapter.h"
#include "NxSubsystems/Graphics/GraphicsComponents.h"

namespace NX
{
    class BaseCamera;
    class ShaderProgram;

    CLASS();
    class SkyboxAsset : public BaseAsset
    {
        R_FRIEND_DECL(SkyboxAsset, NX::BaseAsset);

    public:
        inline static const char* fileExtension = ".nxsky";

        ENUM_CLASS();
        enum class Direction
        {
            Right,
            Left,
            Top,
            Bottom,
            Front,
            Back,
        };

    public:
        explicit SkyboxAsset(const Core::StringAtom& logicPath)
            : BaseAsset(logicPath)
        {
        }
        SkyboxAsset(const SkyboxAsset&) = delete;
        SkyboxAsset(SkyboxAsset&&) noexcept = delete;
        SkyboxAsset& operator=(const SkyboxAsset&) = delete;
        SkyboxAsset& operator=(SkyboxAsset&&) noexcept = delete;
        ~SkyboxAsset() override = default;

        void draw(BaseCamera& camera, ShaderProgram& shader);

        [[nodiscard]] const char* getPrefix() const override { return "Skybox"; }

    protected:
        void onLoadRequest() override;
        void onUnloadRequest() override;

    protected:
        NX::InterleavedGraphicsData _gcd;
        ShaderProgram* _shader = nullptr;
        // properties

        FIELD();
        std::array<std::filesystem::path, 6> _paths;

        FIELD();
        bool _isFlipVertically = false;
    };

    using NXSkybox = AssetRef<SkyboxAsset>;
} // namespace NX

#include "SkyboxAsset.generated.h" // added by the code generator. Better don't move it.
