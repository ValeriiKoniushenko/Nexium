// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "StaticMeshBundle.h"

#include "AssimpMisc/AssimpHelper.h"
#include "Foundation/Configs.h"
#include "NxWorld/Framework/GameInstance.h"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"

namespace NX::AssetImpl
{

    nlohmann::json StaticMeshBundle::AssetData::serialize() const
    {
        return R<AssetData>::Serialize<RJsonResourceStream>(*this).getData();
    }

    void StaticMeshBundle::AssetData::deserialize(RResourceStream<RJsonResourceStream>& data)
    {
        _assimpPostProcessFlagsAsInt = 0;

        R<AssetData>::Deserialize(data, *this);
        for (auto&& asString : assimpPostProcess)
        {
            if (auto flag = Assimp::aiPostProcessStepsFromString(asString.c_str()))
            {
                _assimpPostProcessFlagsAsInt |= flag.value();
            }
        }

        mainShader = Core::StringAtom::Intern(mainShader);
        outlineShader = Core::StringAtom::Intern(outlineShader);
    }

    void StaticMeshBundle::load(const ECSAsset& asset, BaseComponent* dataOwner,
                                const nlohmann::json& assetData)
    {
        if (!dataOwner)
        {
            return;
        }

        AssetData extractedData;
        RResourceStream<RJsonResourceStream> data(assetData);
        extractedData.deserialize(data);
        if (!data.logs().empty())
        {
            gGlobalLog.warnLog("{} field(s) couldn't be loaded for specific asset: {}"_f
                               << data.logs().size() << dataOwner->getComponentName());
            for (auto&& [field, code] : data.logs())
            {
                gGlobalLog.warnLog("Field '{}' - {}"_f << field << RStatusToString(code));
            }
        }

        if (extractedData.meshPath.empty())
        {
            return;
        }

        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(
            (Config::Path::projectAbsPath / extractedData.meshPath).generic_string(),
            extractedData.getAssimpPostProcessFlagsAsInt());
        if (Verify(scene) && Verify(scene->mRootNode))
        {
            if (auto* owner = dataOwner->castTo<Core::StaticMeshBundle>())
            {
                static auto* sm = GetShaderManager();
                owner->importFrom(scene->mRootNode, scene,
                                  Config::Path::projectAbsPath / extractedData.meshPath,
                                  extractedData.onLoadScale);
                if (!extractedData.mainShader.isEmpty())
                {
                    owner->setShader(sm->getShaderProgram(extractedData.mainShader));
                }
                if (!extractedData.outlineShader.isEmpty())
                {
                    owner->setOutlineShader(sm->getShaderProgram(extractedData.outlineShader));
                }
            }
        }
    }

    void StaticMeshBundle::unload(const ECSAsset& asset, BaseComponent* dataOwner)
    {
    }

} // namespace NX::AssetImpl
