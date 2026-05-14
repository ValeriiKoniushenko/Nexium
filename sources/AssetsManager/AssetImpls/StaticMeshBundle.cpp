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

#include "StaticMeshBundle.h"

#include "AssimpMisc/AssimpHelper.h"
#include "GameplaySystem/Framework/GameInstance.h"
#include "Misc/Configs.h"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"

namespace Core::AssetImpl
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

        mainShader = StringAtom::Intern(mainShader);
        outlineShader = StringAtom::Intern(outlineShader);
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
            globalLog.warnLog("{} field(s) couldn't be loaded for specific asset: {}"_f
                              << data.logs().size() << dataOwner->getComponentName());
            for (auto&& [field, code] : data.logs())
            {
                globalLog.warnLog("Field '{}' - {}"_f << field << RStatusToString(code));
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
                static auto& sm = GetShaderManager();
                owner->importFrom(scene->mRootNode, scene,
                                  Config::Path::projectAbsPath / extractedData.meshPath,
                                  extractedData.onLoadScale);
                if (!extractedData.mainShader.isEmpty())
                {
                    owner->setShader(sm.getShaderProgram(extractedData.mainShader));
                }
                if (!extractedData.outlineShader.isEmpty())
                {
                    owner->setOutlineShader(sm.getShaderProgram(extractedData.outlineShader));
                }
            }
        }
    }

    void StaticMeshBundle::unload(const ECSAsset& asset, BaseComponent* dataOwner)
    {
    }

    bool StaticMeshBundle::processAction(AssetAction action, ECSAsset& asset)
    {
        if (ECSAssetImpl::processAction(action, asset))
        {
            return true;
        }

        if (action & AssetAction::AA_Spawn)
        {
            spawn(asset);
            return true;
        }

        return false;
    }
    bool StaticMeshBundle::canProcessAction(AssetAction action) const
    {
        if (action & AssetAction::AA_Spawn)
        {
            return true;
        }

        return ECSAssetImpl::canProcessAction(action);
    }

    void StaticMeshBundle::spawn(ECSAsset& asset)
    {
        gGameInstance->gameScene.addToScene(asset);
    }

} // namespace Core::AssetImpl