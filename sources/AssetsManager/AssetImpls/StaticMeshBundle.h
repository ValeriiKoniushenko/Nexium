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

#include "../ECSAsset.h"
#include "Graphics/Primitives/StaticMeshBundle.h"

namespace Core::AssetImpl
{

    class StaticMeshBundle : public ECSAssetImpl
    {
    public:
        using implementedAssetType = Core::StaticMeshBundle;

        struct AssetData
        {
            AssetData() = default;
            ~AssetData() = default;
            AssetData(const AssetData&) = default;
            AssetData(AssetData&&) noexcept = default;

            std::filesystem::path meshPath;
            StringAtom mainShader;
            StringAtom outlineShader;
            float onLoadScale = 1.0f;
            int assimpPostProcess = 0;
        };

    public:
        void load(const ECSAsset& asset, BaseComponent* dataOwner,
                  const nlohmann::json& assetData) override;
        void unload(const ECSAsset& asset, BaseComponent* dataOwner) override;

        bool processAction(AssetAction action, ECSAsset& asset) override;
        [[nodiscard]] bool canProcessAction(AssetAction action) const override;

    private:
        void spawn(ECSAsset& asset);

    private:
        [[nodiscard]] AssetData extractAssetData(const nlohmann::json& assetData) const;
    };

} // namespace Core::AssetImpl
#include "StaticMeshBundle.generated.h" // added by the code generator. Better don't move it.
