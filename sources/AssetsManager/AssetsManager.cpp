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

#include "AssetsManager.h"

#include "Editor/Configs.h"
#include "GameplaySystem/Framework/GameInstance.h"
#include "Mesh3DAsset.h"

#include <Utils/Functions.h>

namespace Core
{
    AssetsManager::AssetsManager()
    {
        registerNewAssetPath(Config::Path::assets);
    }

    void AssetsManager::rescanFileSystem()
    {
        _textures.clear();

        for (auto&& path : _registeredPaths)
        {
            for (const auto& entry : std::filesystem::recursive_directory_iterator(path))
            {
                if (!entry.is_regular_file())
                {
                    continue;
                }

                const auto absPath = std::filesystem::absolute(entry.path());
                const auto ext = absPath.extension().generic_string();

                // check for non baked
                if (ext.size() < 3 || strncmp(ext.c_str(), ".nx", 3) != 0)
                {
                    continue;
                }

                auto id = StringAtom::Intern(entry.path().generic_string());
                if (ext == NXTexture::AssetT::fileExtension)
                {
                    _textures.emplace(id, new TextureAsset(id))
                        .first->second->onFillData(nlohmann::json::parse(
                            Utils::GetTextFileContentAs<std::string>(absPath)));
                }
                else if (ext == NXSkybox::AssetT::fileExtension)
                {
                    _skyboxes.emplace(id, new SkyboxAsset(id))
                        .first->second->onFillData(nlohmann::json::parse(
                            Utils::GetTextFileContentAs<std::string>(absPath)));
                }
                else if (ext == NXMesh3D::AssetT::fileExtension)
                {
                    _mesh3ds.emplace(id, new Mesh3DAsset(id))
                        .first->second->onFillData(nlohmann::json::parse(
                            Utils::GetTextFileContentAs<std::string>(absPath)));
                }
            }
        }
    }

    NXTexture AssetsManager::getTexture(const StringAtom& logicPath)
    {
        if (!validatePath(logicPath, NXTexture::AssetT::fileExtension))
        {
            return NXTexture();
        }

        if (!_textures.contains(logicPath))
        {
            throw std::runtime_error("Texture not found!");
        }

        return NXTexture(reinterpret_cast<TextureAsset&>(*_textures.at(logicPath)));
    }

    NXSkybox AssetsManager::getSkybox(const StringAtom& logicPath)
    {
        if (!validatePath(logicPath, NXSkybox::AssetT::fileExtension))
        {
            return NXSkybox();
        }

        if (!_skyboxes.contains(logicPath))
        {
            criticalLog("Skybox not found by the next path: {}"_f << logicPath);
            return NXSkybox();
        }

        return NXSkybox(reinterpret_cast<SkyboxAsset&>(*_skyboxes.at(logicPath)));
    }

    NXMesh3D AssetsManager::getMesh3D(const StringAtom& logicPath)
    {
        if (!validatePath(logicPath, NXMesh3D::AssetT::fileExtension))
        {
            return NXMesh3D();
        }

        if (!_mesh3ds.contains(logicPath))
        {
            criticalLog("Mesh3D not found by the next path: {}"_f << logicPath);
            return NXMesh3D();
        }

        return NXMesh3D(reinterpret_cast<Mesh3DAsset&>(*_mesh3ds.at(logicPath)));
    }

    void AssetsManager::unloadAllResources()
    {
        _textures.clear();
        _skyboxes.clear();
    }

    void AssetsManager::registerNewAssetPath(std::filesystem::path path)
    {
        infoLog("Registered new asset path: " + path.generic_string());
        _registeredPaths.emplace(std::move(path));
    }

    void AssetsManager::spawnMesh3DOnScene(const StringAtom& logicPath)
    {
        if (auto&& mesh = getMesh3D(logicPath))
        {
            gGameInstance->gameScene.addActor(mesh);
        }
    }

    bool AssetsManager::validatePath(const StringAtom& logicPath, const char* requiredExt)
    {
        if (logicPath.isEmpty() || !requiredExt)
        {
            ASSERT(false);
            traceLog("Path is empty");
            return false;
        }

        ASSERT(logicPath.isStatic(), "You must use only _atom strings!");

        auto* found = logicPath.reverseFind(".nx");
        if (!found)
        {
            ASSERT(false);
            traceLog("Invalid file extension was passed");
            return false;
        }

        if (strcmp(found, requiredExt) != 0)
        {
            ASSERT(false);
            traceLog("Path is invalid. Expected extension is {} but passed {}"_f << requiredExt
                                                                                 << found);
            return false;
        }

        return true;
    }
} // namespace Core