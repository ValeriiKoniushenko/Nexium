// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "SceneManager.h"

#include "NxFundamental/ResourceManagement/DataStream.h"
#include "NxWorld/PrivateModuleInfo.h"

#include <algorithm>
#include <string>

namespace NX
{
    SceneManager::SceneManager()
    {
        removeAllScenes();
    }

    Scene& SceneManager::createNewScene()
    {
        auto scene = std::make_unique<Scene>();
        std::size_t suffix = 1;
        Core::StringAtom name;
        do
        {
            name = Core::StringAtom::Intern("Scene_" + std::to_string(suffix++));
        } while (getScene(name));
        scene->setSceneName(name);
        scene->initialize();
        _scenes.push_back(std::move(scene));
        GetCacheSystem().write(*_scenes.back());
        return *_scenes.back();
    }

    bool SceneManager::renameScene(Scene* scene, Core::StringAtom newName)
    {
        if (!scene || newName.isEmpty() || (getScene(newName) && getScene(newName) != scene))
        {
            return false;
        }
        if (std::ranges::find_if(_scenes,
                                 [scene](const auto& owned) { return owned.get() == scene; })
            == _scenes.end())
        {
            return false;
        }

        const auto oldName = scene->getSceneName();
        if (oldName == newName)
        {
            return true;
        }
        GetCacheSystem().clearCache(*scene);
        scene->setSceneName(newName);
        GetCacheSystem().write(*scene);
        return true;
    }

    Scene* SceneManager::getScene(const Core::StringAtom& sceneName) const
    {
        const auto it = std::ranges::find_if(_scenes, [&sceneName](const auto& scene)
                                             { return scene->getSceneName() == sceneName; });
        return it != _scenes.end() ? it->get() : nullptr;
    }

    bool SceneManager::setCurrentScene(const Core::StringAtom& sceneName)
    {
        return setCurrentScene(getScene(sceneName));
    }

    bool SceneManager::setCurrentScene(Scene* scene)
    {
        if (scene
            && std::ranges::any_of(_scenes,
                                   [scene](const auto& owned) { return owned.get() == scene; }))
        {
            if (!isSceneOpen(scene))
            {
                _openScenes.push_back(scene);
            }
            if (_currentScene != scene)
            {
                _currentScene = scene;
                onCurrentSceneChanged->trigger(_currentScene);
            }
            return true;
        }
        return false;
    }

    bool SceneManager::isSceneOpen(const Scene* scene) const
    {
        return std::ranges::find(_openScenes, scene) != _openScenes.end();
    }

    bool SceneManager::closeScene(Scene* scene)
    {
        const auto it = std::ranges::find(_openScenes, scene);
        if (it == _openScenes.end() || _openScenes.size() == 1)
        {
            return false;
        }
        const auto index = static_cast<std::size_t>(it - _openScenes.begin());
        _openScenes.erase(it);
        if (_currentScene == scene)
        {
            setCurrentScene(_openScenes[std::min(index, _openScenes.size() - 1)]);
        }
        return true;
    }

    void SceneManager::removeScene(Core::StringAtom sceneName)
    {
        removeScene(getScene(sceneName));
    }

    void SceneManager::removeScene(Scene* scene)
    {
        if (_scenes.size() == 1)
        {
            // At least one scene must always exist.
            return;
        }
        const auto it = std::ranges::find_if(_scenes, [scene](const auto& owned)
                                             { return owned.get() == scene; });
        if (it == _scenes.end())
        {
            return;
        }
        GetCacheSystem().clearCache(*scene);
        std::erase(_openScenes, scene);
        if (scene == _currentScene)
        {
            auto* replacement = _openScenes.empty()
                                    ? (it == _scenes.begin() ? _scenes[1] : _scenes.front()).get()
                                    : _openScenes.front();
            setCurrentScene(replacement);
        }
        _scenes.erase(it);
    }

    void SceneManager::removeAllScenes()
    {
        std::vector<std::unique_ptr<Scene>> replacement;
        auto scene = std::make_unique<Scene>();
        replacement.push_back(std::move(scene));
        _scenes.swap(replacement);
        _currentScene = _scenes.front().get();
        _openScenes = { _currentScene };
        onCurrentSceneChanged->trigger(_currentScene);
    }

    spdlog::logger* SceneManager::getLogger() const
    {
        return NxWorld::getLogger();
    }

} // namespace NX
