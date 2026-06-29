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
#include "ModuleInfo.h"

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

        for (auto&& object : _sceneObjects)
        {
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

    void Scene::writeToCacheSeparateData()
    {
        // for (auto&& asset : _objects)
        {
            // if (auto* object = asset->getData())
            {
                // object->writeToCache();
            }
        }
    }

    void Scene::internal_addObjectToScene(SceneObject* object)
    {
        auto name = object->getComponentName();
        name.trim(' ');

        if (name.isEmpty())
        {
            name = "Unnamed object #{}"_f << ++_uniqueCounterName;
            object->setComponentName(name);
        }
        else if (name.size() != object->getComponentName().size())
        {
            object->setComponentName(name);
        }

        _sceneObjects.emplace_back(object);
        onObjectAdded->trigger(object);
    }

    void Scene::addUniqueObjectToScene(SceneObject::Ptr object)
    {
        for (auto&& obj : _sceneObjects)
        {
            if (obj->getComponentName() == object->getComponentName())
            {
                return;
            }
        }

        addObjectToScene(std::move(object));
    }

    void Scene::addObjectToScene(SceneObject::Ptr object)
    {
        internal_addObjectToScene(object.get());

        infoLog("Object '{} was spawned at the scene: '{}'"_f << object->stringify() << _sceneName);
    }

    void Scene::addBlueprintObjectToScene(const WeakData<ECSAsset>& asset, const StringAtom& name)
    {
        const auto& meta = asset->getMeta();

        auto finalAsset = GetAssetsManager().getUniqueEcsAsset(meta.logicPath);
        SceneObject::Ptr obj = DynamicCast<SceneObject>(finalAsset);
        if (!obj)
        {
            errorLog("Blueprint '{}' isn't SceneObject. Impossible to add it to the scene."_f
                     << meta.logicPath);
            return;
        }

        obj->setComponentName(name);
        internal_addObjectToScene(obj.get());

        infoLog("Blueprint's object '{}' was spawned at the scene: '{}'"_f << obj->stringify()
                                                                           << _sceneName);
    }

    [[nodiscard]] nlohmann::json Scene::serialize() const
    {
        auto json = R<Scene>::Serialize(*this).getData();

        json[StreamData::sceneObjects] = nlohmann::json::array();
        for (const auto& obj : _sceneObjects)
        {
            // json[StreamData::sceneObjects].push_back(obj->getSceneState());
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

            internal_addObjectToScene(sceneObj);
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
        return ::Scene::getLogger();
    }

    void Scene::initialize()
    {
        skybox = GetAssetsManager().getSkybox("data/assets/baked/skybox/default.nxsky"_atom);
    }

    void Scene::tick(float timeDelta)
    {
        for (auto&& object : _sceneObjects)
        {
            object->tick(timeDelta);
        }
    }
} // namespace Core
