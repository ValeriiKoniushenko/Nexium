// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "SceneManager.h"

#include "Foundation/Debug/Latency.h"
#include "NxFundamental/ResourceManagement/DataStream.h"
#include "NxWorld/PrivateModuleInfo.h"

#include <algorithm>
#include <iterator>
#include <string>

namespace NX
{
    SceneManager::SceneManager()
    {
        resetScenes();
    }

    SceneManager::~SceneManager()
    {
        _currentScene->deactivate();
        for (auto* scene : _openScenes)
        {
            scene->deinitialize();
        }
    }

    void SceneManager::restoreOpenedScenes(std::vector<Scene*> opened, Scene* current)
    {
        NX_LATENCY_POINT("Restore opened scenes - start");
        for (auto* scene : opened)
        {
            scene->initialize();
        }
        auto previouslyOpened = std::move(_openScenes);
        _openScenes = std::move(opened);
        if (_currentScene)
        {
            _currentScene->deactivate();
        }
        _currentScene = current;
        _currentScene->activate();
        onCurrentSceneChanged->trigger(_currentScene);
        for (auto* scene : previouslyOpened)
        {
            if (!isSceneOpen(scene))
            {
                scene->deinitialize();
            }
        }
        NX_LATENCY_POINT("Restore opened scenes - end");
    }

    Scene& SceneManager::createNewScene()
    {
        auto scene = Core::IntrusivePtr<Scene>(new Scene());
        std::size_t suffix = 1;
        Core::StringAtom name;
        do
        {
            name = Core::StringAtom::Intern("Scene_" + std::to_string(suffix++));
        } while (getScene(name));
        scene->setSceneName(name);
        _scenes.push_back(std::move(scene));
        GetCacheSystem().write(*_scenes.back());
        return *_scenes.back();
    }

    bool SceneManager::renameScene(Scene* scene, const Core::StringAtom& newName)
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
                scene->initialize();
                _openScenes.push_back(scene);
            }
            if (_currentScene != scene)
            {
                _currentScene->deactivate();
                _currentScene = scene;
                _currentScene->activate();
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
        const auto next = _openScenes.erase(it);
        if (_currentScene == scene)
        {
            setCurrentScene(next == _openScenes.end() ? _openScenes.back() : *next);
        }
        scene->deinitialize();
        return true;
    }

    bool SceneManager::removeScene(const Core::StringAtom& sceneName)
    {
        return removeScene(getScene(sceneName));
    }

    bool SceneManager::removeScene(Scene* scene)
    {
        if (_scenes.size() == 1)
        {
            // At least one scene must always exist.
            return false;
        }
        const auto it = std::ranges::find_if(_scenes, [scene](const auto& owned)
                                             { return owned.get() == scene; });
        if (it == _scenes.end())
        {
            return false;
        }
        GetCacheSystem().clearCache(*scene);
        auto* replacement = _currentScene;
        const auto opened = std::ranges::find(_openScenes, scene);
        if (opened != _openScenes.end())
        {
            const auto next = _openScenes.erase(opened);
            if (scene == _currentScene && !_openScenes.empty())
            {
                replacement = next == _openScenes.end() ? _openScenes.back() : *next;
            }
        }
        if (scene == _currentScene)
        {
            if (_openScenes.empty())
            {
                const auto next = std::next(it);
                replacement = (next == _scenes.end() ? _scenes.front() : *next).get();
            }
            setCurrentScene(replacement);
        }
        scene->deinitialize();
        _scenes.erase(it);
        return true;
    }

    void SceneManager::resetScenes()
    {
        std::vector<Core::IntrusivePtr<Scene>> replacement;
        auto scene = Core::IntrusivePtr<Scene>(new Scene());
        replacement.push_back(std::move(scene));
        _scenes.swap(replacement);
        restoreOpenedScenes({ _scenes.front().get() }, _scenes.front().get());
    }

    spdlog::logger* SceneManager::getLogger() const
    {
        return NxWorld::getLogger();
    }

} // namespace NX
