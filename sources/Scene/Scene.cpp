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

namespace
{
    using ObjectContainerT = Core::Scene::ObjectContainerT;
    using SceneObject = Core::SceneObject;

    // Returns true if `name` exists among objects' component names.
    bool nameExists(const ObjectContainerT& objects, const Core::StringAtom& name)
    {
        for (auto&& obj : objects)
        {
            if (obj->getComponentName() == name)
            {
                return true;
            }
        }
        return false;
    }

    // Finds a unique name for `obj` given existing `objects`, and sets it.
    void unifyObjectName(const ObjectContainerT& objects, SceneObject* obj)
    {
        const auto baseName = obj->getComponentName();

        if (!nameExists(objects, baseName))
        {
            return; // already unique, nothing to do
        }

        // baseName is taken -> find next free suffix N in "baseName_N"
        int suffix = 0;
        Core::StringAtom candidate;
        do
        {
            candidate = baseName + "_" + Core::StringAtom::MakeFrom(suffix);
            ++suffix;
        } while (nameExists(objects, candidate));

        obj->setComponentName(candidate);
    }

} // namespace

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

    void Scene::internal_addObjectToScene(SceneObject* object)
    {
        auto name = object->getComponentName();
        name.trim(' ');

        if (name.isEmpty())
        {
            name = "{}_{}"_f << object->getComponentType() << ++_uniqueCounterName;
            object->setComponentName(name);
        }
        else if (name.size() != object->getComponentName().size())
        {
            object->setComponentName(name);
        }

        // TODO: optimize it! Absolutely slow.
        unifyObjectName(_sceneObjects, object);
        _sceneObjects.emplace_back(object);
        object->initialize();

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

        infoLog("Object '{}' was spawned at the scene: '{}'"_f << object->stringify()
                                                               << _sceneName);
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

        obj->_setReferencedAsset(meta.logicPath);
        if (obj->getComponentName().isEmpty())
        {
            obj->setComponentName(name);
        }

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
            const SceneState states = value.get<SceneState>();

            auto&& refAsset = GetAssetsManager().getUniqueEcsAsset(states.referenceAsset);
            if (!refAsset)
            {
                errorLog(
                    "Scene: '{}'. Impossible to spawn an object '{}'({}) to the scene, object is not accessible through AssetManager."_f
                    << _sceneName << states.referenceAsset << states.name);
                continue;
            }

            auto* sceneObj = dynamic_cast<SceneObject*>(refAsset.get());
            if (!sceneObj)
            {
                errorLog(
                    "Scene object isn't SceneObject. Impossible to add it to the scene. Asset type is: {}; name is: {}"_f
                    << states.assetType << states.name);
                continue;
            }

            sceneObj->setTransformations(states.trans);

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
