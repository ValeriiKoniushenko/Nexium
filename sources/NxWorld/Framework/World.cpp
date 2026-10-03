// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "World.h"

#include "NxWorld/Entities/Camera/Camera.h"
#include "NxWorld/PrivateModuleInfo.h"
#include "NxWorld/Scene/Scene/Scene.h"

namespace NX
{

    void World::bindScene(Scene* scene)
    {
        _sceneSubscriptionPool.clearAndReleaseAll();
        _boundScene = scene;
        updateCurrentCamera(nullptr);
        if (!scene)
        {
            return;
        }

        _sceneSubscriptionPool << scene->onObjectAdded->subscribeAndGetID(
            [this](SceneObject* object) { internal_onAddObjectToScene(object); });
        updateCurrentCamera(scene->getMainCamera());
        for (const auto& object : scene->getObjects())
        {
            internal_onAddObjectToScene(object.get());
        }
    }

    void World::setCurrentCamera(BaseCamera* camera)
    {
        if (_boundScene)
        {
            _boundScene->setMainCamera(camera);
        }
        updateCurrentCamera(camera);
    }

    void World::resetCamera()
    {
        setCurrentCamera(nullptr);
    }

    void World::updateCurrentCamera(BaseCamera* camera)
    {
        if (_currentCamera == camera)
        {
            return;
        }
        _currentCamera = camera;
        onCurrentCameraChanged->trigger(camera);
    }

    void World::internal_onAddObjectToScene(SceneObject* object)
    {
        if (_currentCamera)
        {
            return;
        }
        object->forEach(
            [this](BaseComponent* component)
            {
                if (auto* camera = dynamic_cast<BaseCamera*>(component))
                {
                    setCurrentCamera(camera);
                    return false;
                }
                return true;
            });
    }

    std::filesystem::path LightningProps::getCacheDir() const
    {
        return "LightningProps";
    }

    Core::StringAtom LightningProps::getCacheHash() const
    {
        return "LightningProps";
    }

    spdlog::logger* World::getLogger() const
    {
        return NxWorld::getLogger();
    }

    void World::internal_UpdateTimeDelta(float delta) noexcept
    {
        _timeDelta = delta;
        _activeTime += delta;
    }

    std::filesystem::path World::getCacheDir() const
    {
        return "world";
    }

    Core::StringAtom World::getCacheHash() const
    {
        return worldName;
    }

} // namespace NX
