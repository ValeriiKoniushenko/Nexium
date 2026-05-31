/*
 * MIT License
 *
 * Copyright (c) 2018-2027 Valerii Koniushenko
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

#include "Scene.h"

#include "GameplaySystem/Entities/Actor.h"
#include "GameplaySystem/Framework/GameInstance.h"
#include "Graphics/Primitives/StaticMeshBundle.h"

namespace Core
{

    void Scene::directDraw()
    {
        if (!gGameInstance->currentCamera)
        {
            return;
        }

        auto& camera = *gGameInstance->currentCamera;

        if (gGameInstance->renderMode == GameInstance::RenderMode::Editor)
        {
            gGameInstance->gameEditor.slowObjectPicker.update(*this);
        }

        grid.draw();

        _postDrawBuffer.resize(0);

        for (auto&& asset : _objects)
        {
            auto* object = asset->getData();

            if (!object->isEnabled())
            {
                continue;
            }

            if (auto* mesh = object->tryCastTo<StaticMeshBundle>())
            {
                if (!mesh->isPostDraw())
                {
                    mesh->draw(camera);
                }
                else
                {
                    _postDrawBuffer.push_back(mesh);
                }
            }
        }

        for (auto& obj : _sceneObjects)
        {
            obj->draw(camera);
        }

        for (auto&& mesh : _postDrawBuffer)
        {
            mesh->draw(camera);
        }

        skybox->draw(camera);
    }

    void Scene::setSceneName(StringAtom name)
    {
        if (Verify(!name.isEmpty()))
        {
            _sceneName = std::move(name);
        }
    }

    const StringAtom& Scene::getSceneName() const noexcept
    {
        return _sceneName;
    }

    void Scene::addToScene(const BaseComponent* object, bool readFromCache)
    {
        if (!Verify(object)) [[unlikely]]
        {
            return;
        }

        _objects.emplace_back(new WorldObject);
        auto* sceneAsset = _objects.back().get();

        if (!sceneAsset->setData(object))
        {
            return;
        }

        auto* added = sceneAsset->getData();

        added->initialize();
        if (readFromCache)
        {
            // added->tryReadFromCache();
        }

        onObjectAdded->trigger(sceneAsset);
    }

    void Scene::addToScene(ECSAsset& asset, bool readFromCache)
    {
        _objects.emplace_back(new WorldObject);
        auto* sceneAsset = _objects.back().get();
        sceneAsset->setAsset(asset);

        if (auto* added = sceneAsset->getData(); Verify(added)) [[likely]]
        {
            added->initialize();

            if (readFromCache)
            {
                // added->tryReadFromCache();
            }

            onObjectAdded->trigger(sceneAsset);
        }
    }

    void Scene::writeToCacheSeparateData()
    {
        for (auto&& asset : _objects)
        {
            // if (auto* object = asset->getData())
            {
                // object->writeToCache();
            }
        }
    }

    [[nodiscard]] nlohmann::json Scene::serialize() const
    {
        auto json = R<Scene>::Serialize(*this).getData();

        json[StreamData::sceneObjects] = nlohmann::json::array();
        for (const auto& obj : _sceneObjects)
        {
            json[StreamData::sceneObjects].push_back(obj->getSceneState());
        }

        return json;
    }

    void Scene::deserialize(RResourceStream<RJsonResourceStream>& data)
    {
        R<Scene>::Deserialize(data, *this);

        auto&& arr = data.getData()[StreamData::sceneObjects];

        for (const auto& [_, value] : arr.items())
        {
            SceneState states = value.get<SceneState>();

            auto* obj = GetGlobalComponentFactory().create(StringAtom::Intern(states.assetType));
            auto* sceneObj = dynamic_cast<SceneObject*>(obj);
            if (!sceneObj)
            {
                errorLog(
                    "Scene object isn't SceneObject. Impossible to add it to the scene. Asset type is: {}; name is: {}"_f
                    << states.assetType << states.name);
                continue;
            }
            _sceneObjects.emplace_back(sceneObj);
        }
    }

    std::filesystem::path Scene::getCacheDir() const
    {
        return "scenes";
    }

    StringAtom Scene::getCacheHash() const
    {
        return _sceneName;
    }

    spdlog::logger* Scene::getLogger() const
    {
        return Scene::getLogger();
    }

    void Scene::initialize()
    {
        skybox = GetAssetsManager().getSkybox("data/assets/baked/skybox/default.nxsky"_atom);
    }

    void Scene::tick(float timeDelta)
    {
        for (auto&& asset : _objects)
        {
            if (auto* object = asset->getData()) [[likely]]
            {
                object->tick(timeDelta);
            }
        }
    }
} // namespace Core
