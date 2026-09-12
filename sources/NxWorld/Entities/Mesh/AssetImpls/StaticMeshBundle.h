// Nexium
// Copyright 2018-2027 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "../ECSAsset.h"
#include "NxWorld/Entities/Mesh/StaticMeshBundle.h"

namespace NX::AssetImpl
{

    class StaticMeshBundle : public ECSAssetImpl
    {
    public:
        using implementedAssetType = Core::StaticMeshBundle;

        CLASS();
        class AssetData
        {
        public:
            R_FRIEND(AssetData);

            FIELD();
            std::filesystem::path meshPath;

            FIELD();
            Core::StringAtom mainShader;

            FIELD();
            Core::StringAtom outlineShader;

            FIELD();
            float onLoadScale = 1.0f;

            FIELD();
            std::vector<std::string> assimpPostProcess;

            [[nodiscard]] int getAssimpPostProcessFlagsAsInt() const noexcept
            {
                return _assimpPostProcessFlagsAsInt;
            }

            [[nodiscard]] nlohmann::json serialize() const;
            void deserialize(RResourceStream<RJsonResourceStream>& data);

        private:
            int _assimpPostProcessFlagsAsInt = 0;
        };

    public:
        void load(const ECSAsset& asset, BaseComponent* dataOwner,
                  const nlohmann::json& assetData) override;
        void unload(const ECSAsset& asset, BaseComponent* dataOwner) override;
    };

} // namespace NX::AssetImpl

#include "StaticMeshBundle.generated.h" // added by the code generator. Better don't move it.
