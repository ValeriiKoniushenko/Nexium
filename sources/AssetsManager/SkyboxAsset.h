/*
 * MIT License
 *
 * Copyright (c) 2018-2025 Valerii Koniushenko
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

#include "BaseAsset.h"
#include "Graphics/GraphicsComponents.h"
#include "RawDataManagement/JsonAdapter.h"

namespace Core
{
    class BaseCamera;

    CLASS();
    class [[deprecated]] SkyboxAsset : public BaseAsset
    {
        R_FRIEND_DECL(SkyboxAsset, Core::BaseAsset);

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
        explicit SkyboxAsset(const StringAtom& logicPath)
            : BaseAsset(logicPath)
        {
        }
        SkyboxAsset(const SkyboxAsset&) = default;
        SkyboxAsset(SkyboxAsset&&) = default;
        SkyboxAsset& operator=(const SkyboxAsset&) = default;
        SkyboxAsset& operator=(SkyboxAsset&&) = default;
        ~SkyboxAsset() override = default;

        void draw(BaseCamera& camera);

        [[nodiscard]] const char* getPrefix() const override { return "Skybox"; }

    protected:
        void onLoadRequest() override;
        void onUnloadRequest() override;

    protected:
        InterleavedGraphicsData _gcd;
        // properties

        FIELD();
        std::array<std::filesystem::path, 6> _paths;

        FIELD();
        bool _isFlipVertically = false;
    };

    using NXSkybox = AssetRef<SkyboxAsset>;
} // namespace Core

#include "SkyboxAsset.generated.h" // added by the code generator. Better don't move it.
