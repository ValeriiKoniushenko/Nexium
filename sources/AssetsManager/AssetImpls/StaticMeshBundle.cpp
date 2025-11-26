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

#include "Editor/Windows/ECSAdapters/EditorStaticMeshBundleAdapter.h"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"

namespace Core::AssetImpl
{

    void StaticMeshBundle::load(const ECSAsset& asset, BaseComponent* dataOwner,
                                const nlohmann::json& assetData)
    {
        if (!dataOwner)
        {
            return;
        }

        const auto extractedData = extractAssetData(assetData);
        if (extractedData.meshPath.empty())
        {
            return;
        }

        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(
            (Config::Path::projectAbsPath / extractedData.meshPath).generic_string(),
            extractedData.assimpPostProcess);
        if (Verify(scene) && Verify(scene->mRootNode))
        {
            if (const auto owner = dynamic_cast<Core::StaticMeshBundle*>(dataOwner); Verify(owner))
            {
                static auto& sm = GetShaderManager();
                owner->importFrom(scene->mRootNode, scene,
                                  Config::Path::projectAbsPath / extractedData.meshPath,
                                  extractedData.onLoadScale);
                if (!extractedData.mainShader.isEmpty())
                {
                    owner->setShader(sm.getShaderProgram(extractedData.mainShader));
                }
                if (extractedData.outlineShader.isEmpty())
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
        gGameInstance->gameScene.addAsset(&asset);
    }

    StaticMeshBundle::AssetData StaticMeshBundle::extractAssetData(
        const nlohmann::json& assetData) const
    {
        using Naming = ECSEditorStaticMeshBundleAdapter::AssetData;

        AssetData data;

        if (assetData.contains(Naming::mainShader))
        {
            data.mainShader = StringAtom::Intern(assetData[Naming::mainShader].get<StringAtom>());
        }
        else
        {
            globalLog.errorLog("'mainShader' didn't find while parsing the asset.");
        }

        if (assetData.contains(Naming::outlineShader))
        {
            data.outlineShader
                = StringAtom::Intern(assetData[Naming::outlineShader].get<StringAtom>());
        }
        else
        {
            globalLog.errorLog("'outlineShader' didn't find while parsing the asset.");
        }

        if (assetData.contains(Naming::onLoadScale))
        {
            data.onLoadScale = assetData[Naming::onLoadScale].get<float>();
        }
        else
        {
            globalLog.errorLog("'onLoadScale' didn't find while parsing the asset.");
        }

        if (assetData.contains(Naming::path))
        {
            data.meshPath = assetData[Naming::path].get<std::filesystem::path>();
        }
        else
        {
            globalLog.errorLog("'path' didn't find while parsing the asset.");
        }

        if (assetData.contains(Naming::assimpPostProcess))
        {
            for (auto&& asString : assetData[Naming::assimpPostProcess].items())
            {
                if (auto flag = Assimp::aiPostProcessStepsFromString(asString.value()))
                {
                    data.assimpPostProcess |= flag.value();
                }
            }
        }
        else
        {
            globalLog.errorLog("'assimpPostProcess' didn't find while parsing the asset.");
        }

        return data;
    }

} // namespace Core::AssetImpl