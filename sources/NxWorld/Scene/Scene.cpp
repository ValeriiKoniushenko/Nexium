// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "Scene.h"

#include "NxWorld/Entities/Actor.h"
#include "NxWorld/Entities/Camera/Camera.h"
#ifdef NEXIUM_ENABLE_3D_MODULE
    #include "NxWorld/Entities/Mesh/StaticMeshBundle.h"
#endif
#include "NxWorld/Framework/GameInstance.h"
#include "NxWorld/PrivateModuleInfo.h"

#include <stdexcept>

namespace
{
    using ObjectContainerT = NX::Scene::ObjectContainerT;
    using SceneObject = NX::SceneObject;

    void validateSceneComponent(const nlohmann::json& data)
    {
        const auto type = Core::StringAtom::Intern(data.at("_type").get<Core::StringAtom>());
        if (!NX::GetGlobalComponentFactory().containsSuchType(type))
        {
            throw std::runtime_error("Unregistered scene component type: " + type.toStdString());
        }
        if (data.contains("_children") && !data["_children"].is_null())
        {
            if (!data["_children"].is_array())
            {
                throw std::runtime_error("Scene component children must be an array.");
            }
            for (const auto& child : data["_children"])
            {
                validateSceneComponent(child);
            }
        }
    }

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

namespace NX
{

    void Scene::directDraw(NX::ShaderProgram* skyboxShader, NX::ShaderProgram* gridShader)
    {
        auto* world = GetWorld();
        if (!world || !world->currentCamera)
        {
            return;
        }

        auto& camera = *world->currentCamera;

        if (gGameInstance->isEditorMode())
        {
            grid.draw(gridShader, &camera);
        }

        _postDrawBuffer.resize(0);

        for (auto&& object : _sceneObjects)
        {
            if (!object->isEnabled())
            {
                continue;
            }

#ifdef NEXIUM_ENABLE_3D_MODULE
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
#endif
        }

        for (auto& obj : _sceneObjects)
        {
            obj->draw(camera);
        }

        for (auto&& mesh : _postDrawBuffer)
        {
            mesh->draw(camera);
        }

        if (world->currentCamera->getType() == CameraType::Perspective && skyboxShader)
        {
            skybox->draw(camera, *skyboxShader);
        }
    }

    void Scene::setSceneName(Core::StringAtom name)
    {
        if (Verify(!name.isEmpty()))
        {
            _sceneName = std::move(name);
        }
    }

    const Core::StringAtom& Scene::getSceneName() const noexcept
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

        object->onAddedToScene();
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

        infoLog("Object '{}' was spawned at the scene: '{}'"_f << object->shortStringify()
                                                               << _sceneName);
    }

    void Scene::addBlueprintObjectToScene(const Core::WeakData<ECSAsset>& asset,
                                          const Core::StringAtom& name)
    {
        const auto& meta = asset->getMeta();

        auto finalAsset = GetAssetsManager()->getUniqueEcsAsset(meta.logicPath);
        SceneObject::Ptr obj = Core::DynamicCast<SceneObject>(finalAsset);
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

        infoLog("Blueprint's object '{}' was spawned at the scene: '{}'"_f << obj->shortStringify()
                                                                           << _sceneName);
    }

    bool Scene::deleteFromScene(const Core::StringAtom& name)
    {
        for (auto it = _sceneObjects.begin(); it != _sceneObjects.end(); ++it)
        {
            const auto* obj = it->get();
            if (Verify(obj))
            {
                if (obj->getComponentName() == name)
                {
                    traceLog("The object is deleted from the scene: {}"_f
                             << obj->getComponentName());
                    _sceneObjects.erase(it);
                    return true;
                }
            }
        }

        return false;
    }

    bool Scene::deleteFromScene(const BaseComponent* obj)
    {
        if (!obj)
        {
            errorLog("Can't delete the NULL object from the scene");
            return false;
        }

        for (auto it = _sceneObjects.begin(); it != _sceneObjects.end(); ++it)
        {
            if (it->get() == obj)
            {
                traceLog("The object is deleted from the scene: {}"_f << obj->getComponentName());
                _sceneObjects.erase(it);
                return true;
            }
        }

        return false;
    }

    bool Scene::deleteFromSceneOrFromObject(BaseComponent* obj)
    {
        if (deleteFromScene(obj))
        {
            return true;
        }

        auto* owner = obj->getOwner();
        if (!owner || owner == obj)
        {
            return false;
        }

        for (auto it = _sceneObjects.begin(); it != _sceneObjects.end(); ++it)
        {
            if (it->get() == owner)
            {
                owner->removeChildDeep(obj);

                traceLog("The object is deleted from the scene: {}"_f << obj->getComponentName());
                return true;
            }
        }

        return false;
    }

    void Scene::duplicateSceneObject(const BaseComponent* obj)
    {
        if (auto found = find(obj))
        {
            auto newObj = Core::DynamicCast<SceneObject>(found->clone());
            addObjectToScene(std::move(newObj));
        }
    }

    Core::IntrusivePtr<SceneObject> Scene::find(const BaseComponent* obj)
    {
        for (auto&& o : _sceneObjects)
        {
            if (Verify(o))
            {
                if (o.get() == obj)
                {
                    return o;
                }
            }
        }

        return nullptr;
    }

    [[nodiscard]] nlohmann::json Scene::serialize() const
    {
        auto json = R<Scene>::Serialize(*this).getData();
        json["formatVersion"] = 1;
        json[StreamData::sceneObjects] = nlohmann::json::array();
        for (const auto& obj : _sceneObjects)
        {
            nlohmann::json state = obj->getSceneState();
            if (!obj->hasReferencedAsset())
            {
                state["componentData"] = obj->serialize();
            }
            json[StreamData::sceneObjects].push_back(std::move(state));
        }
        return json;
    }

    void Scene::deserialize(RResourceStream<RJsonResourceStream>& data)
    {
        const auto& json = data.getData();
        const auto version = json.value("formatVersion", nlohmann::json(0));
        if (!version.is_number_integer() || version < 0 || version > 1)
        {
            throw std::runtime_error("Unsupported scene format version: " + version.dump());
        }
        if (!json.contains(StreamData::sceneObjects) || !json[StreamData::sceneObjects].is_array())
        {
            throw std::runtime_error("Scene must contain a sceneObjects array.");
        }

        // Validate and reconstruct before replacing the authored scene.
        Scene replacement;
        R<Scene>::Deserialize(data, replacement);
        if (replacement._sceneName.isEmpty())
        {
            throw std::runtime_error("Scene name must not be empty.");
        }
        for (const auto& value : json[StreamData::sceneObjects])
        {
            const SceneState state = value.get<SceneState>();
            BaseComponent::Ptr component;
            if (!state.referenceAsset.isEmpty())
            {
                auto* assets = GetAssetsManager();
                if (assets)
                {
                    component = assets->getUniqueEcsAsset(state.referenceAsset);
                }
            }
            else if (value.contains("componentData"))
            {
                const auto& componentJson = value["componentData"];
                validateSceneComponent(componentJson);
                const auto type = Core::StringAtom::Intern(state.assetType);
                if (componentJson.at("_type").get<Core::StringAtom>() != type)
                {
                    throw std::runtime_error("Inline scene object type does not match its data.");
                }
                component = GetGlobalComponentFactory().create(type);
                auto stream = RResourceStream<RJsonResourceStream>(componentJson);
                component->deserialize(stream);
            }

            auto object = Core::DynamicCast<SceneObject>(component);
            if (!object || object->getComponentType() != state.assetType)
            {
                errorLog(
                    "It seems that the registered asset/object '{}' on the scene was corrupted in the refrenced file. Absense of the vital information. Referenced asset path: '{}'"_f
                    << (state.name.empty() ? Stringify::gcDefaultNoneString.data() : state.name)
                    << state.referenceAsset);
            }
            const auto name = Core::StringAtom::MakeFrom(state.name);
            if (name.isEmpty() || nameExists(replacement._sceneObjects, name))
            {
                errorLog("Empty or duplicate scene object name: " + state.name);
            }
            object->setComponentName(name);
            object->applyTypeSpecificSceneData(state.typeSpecificData);
            object->setTransformations(state.trans);
            if (!state.referenceAsset.isEmpty())
            {
                object->_setReferencedAsset(state.referenceAsset);
            }
            replacement._sceneObjects.push_back(std::move(object));
        }

        if (gGameInstance && &gGameInstance->gameScene == this)
        {
            gGameInstance->resetCamera();
        }
        _sceneObjects.swap(replacement._sceneObjects);
        _sceneName = std::move(replacement._sceneName);
        _uniqueCounterName = 0;
        _postDrawBuffer.clear();
        replacement._sceneObjects.clear();
        for (auto& object : _sceneObjects)
        {
            object->initialize();
            onObjectAdded->trigger(object.get());
            object->onAddedToScene();
        }
    }

    std::filesystem::path Scene::getCacheDir() const
    {
        return "scenes";
    }

    Core::StringAtom Scene::getCacheHash() const
    {
        return _sceneName;
    }

    spdlog::logger* Scene::getLogger() const
    {
        return NxWorld::getLogger();
    }

    void Scene::initialize()
    {
        // auto asset =
        // GetAssetsManager()->getSkybox("data/assets/baked/skybox/default.nxsky"_atom); skybox =
        // NXSkybox{ dynamic_cast<SkyboxAsset*>(asset.get()) };
        grid.setPlane(glm::vec3(0.0f, 0.0f, -1.f), glm::vec3(0.0f, 0.0f, 1.0f));
    }

    void Scene::tick(float timeDelta)
    {
        for (auto&& object : _sceneObjects)
        {
            object->tick(timeDelta);
        }
    }
} // namespace NX
