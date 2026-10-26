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
#include "NxWorld/Framework/World.h"
#include "NxWorld/PrivateModuleInfo.h"

#include <algorithm>
#include <iterator>
#include <string>
#include <utility>

namespace
{
    // Addresses the file a scene would use under another name.
    class SceneFile final : public Foundation::IDataIO
    {
    public:
        SceneFile(std::filesystem::path directory, Core::StringAtom name)
            : _directory(std::move(directory)),
              _name(std::move(name))
        {
        }

        [[nodiscard]] std::filesystem::path getCacheDir() const override { return _directory; }
        [[nodiscard]] Core::StringAtom getCacheHash() const override { return _name; }

    private:
        std::filesystem::path _directory;
        Core::StringAtom _name;
    };
} // namespace

namespace NX
{
    SceneManager::SceneManager(World* world)
        : _world(world)
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
        if (rejectWhileTicking("restore opened scenes"))
        {
            return;
        }
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
        _currentScene->activate(_world);
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

    void SceneManager::openStartScene()
    {
        const auto startScene = _world ? _world->startScene : Core::StringAtom{};
        auto* scene = startScene.isEmpty() ? nullptr : getScene(startScene);
        if (!scene)
        {
            scene = _scenes.front().get();
            if (!startScene.isEmpty())
            {
                warnLog("Start scene '{}' was not found; starting with '{}'."_f
                        << startScene << scene->getSceneName());
            }
        }
        restoreOpenedScenes({ scene }, scene);
    }

    Scene& SceneManager::createNewScene()
    {
        auto scene = Core::IntrusivePtr<Scene>(new Scene());
        std::size_t suffix = 1;
        Core::StringAtom name;
        do
        {
            name = Core::StringAtom::Intern("Scene_" + std::to_string(suffix++));
        } while (getScene(name) || isSceneFileTaken(*scene, name));
        scene->setSceneName(name);
        _scenes.push_back(std::move(scene));
        writeSceneFile(*_scenes.back());
        return *_scenes.back();
    }

    bool SceneManager::renameScene(Scene* scene, const Core::StringAtom& newName)
    {
        if (!scene || newName.isEmpty() || (getScene(newName) && getScene(newName) != scene))
        {
            return false;
        }
        if (!hasScene(scene))
        {
            return false;
        }

        const auto oldName = scene->getSceneName();
        if (oldName == newName)
        {
            return true;
        }
        if (isSceneFileTaken(*scene, newName))
        {
            return false;
        }
        clearSceneFile(*scene);
        scene->setSceneName(newName);
        writeSceneFile(*scene);
        if (_world && _world->startScene == oldName)
        {
            _world->startScene = newName;
        }
        return true;
    }

    bool SceneManager::isSceneFileTaken(const Scene& scene, const Core::StringAtom& name) const
    {
        const SceneFile target(scene.getCacheDir(), name);
        if (!GetCacheSystem().hasCache(target))
        {
            return false;
        }
        // Different names can map to the same file, which the scene may keep using.
        return !_scenesOwningFile.contains(&scene)
               || GetCacheSystem().getPath(target) != GetCacheSystem().getPath(scene);
    }

    bool SceneManager::writeSceneFile(const Scene& scene, CacheWriteMode mode)
    {
        if (!_scenesOwningFile.contains(&scene) && GetCacheSystem().hasCache(scene))
        {
            errorLog(
                "Scene '{}' was not saved: {} was not loaded by this editor and is kept "
                "untouched. Fix or remove that file to save the scene."_f
                << scene.getSceneName() << GetCacheSystem().getPath(scene).generic_string());
            return false;
        }
        if (!GetCacheSystem().write(scene, mode))
        {
            return false;
        }
        _scenesOwningFile.insert(&scene);
        return true;
    }

    void SceneManager::clearSceneFile(const Scene& scene)
    {
        if (_scenesOwningFile.erase(&scene) != 0)
        {
            GetCacheSystem().clearCache(scene);
        }
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
        if (rejectWhileTicking("change the current scene"))
        {
            return false;
        }
        if (hasScene(scene))
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
                _currentScene->activate(_world);
                onCurrentSceneChanged->trigger(_currentScene);
            }
            return true;
        }
        return false;
    }

    bool SceneManager::hasScene(const Scene* scene) const
    {
        return scene
               && std::ranges::any_of(_scenes,
                                      [scene](const auto& owned) { return owned.get() == scene; });
    }

    bool SceneManager::isSceneOpen(const Scene* scene) const
    {
        return std::ranges::find(_openScenes, scene) != _openScenes.end();
    }

    bool SceneManager::closeScene(Scene* scene)
    {
        if (rejectWhileTicking("close a scene"))
        {
            return false;
        }
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
        if (rejectWhileTicking("remove a scene"))
        {
            return false;
        }
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
        clearSceneFile(*scene);
        if (_world && _world->startScene == scene->getSceneName())
        {
            _world->startScene = Core::StringAtom{};
        }
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

    void SceneManager::requestCurrentScene(const Core::StringAtom& sceneName)
    {
        _pendingChanges.push_back({ PendingChange::Kind::SetCurrent, nullptr, sceneName });
    }

    void SceneManager::requestCurrentScene(Scene* scene)
    {
        _pendingChanges.push_back({ PendingChange::Kind::SetCurrent, scene, {} });
    }

    void SceneManager::requestCloseScene(Scene* scene)
    {
        _pendingChanges.push_back({ PendingChange::Kind::Close, scene, {} });
    }

    void SceneManager::requestRemoveScene(Scene* scene)
    {
        _pendingChanges.push_back({ PendingChange::Kind::Remove, scene, {} });
    }

    void SceneManager::applyPendingChanges()
    {
        if (_pendingChanges.empty() || rejectWhileTicking("apply scene changes"))
        {
            return;
        }
        // Changes requested while applying (e.g. from onCurrentSceneChanged) wait for the next
        // call.
        const auto changes = std::exchange(_pendingChanges, {});
        for (const auto& change : changes)
        {
            if (!applyPendingChange(change))
            {
                const auto& name = change.scene ? change.scene->getSceneName() : change.sceneName;
                warnLog("Requested scene change for '{}' was not applied."_f << name);
            }
        }
    }

    bool SceneManager::applyPendingChange(const PendingChange& change)
    {
        auto* scene = change.scene ? change.scene.get() : getScene(change.sceneName);
        switch (change.kind)
        {
            case PendingChange::Kind::SetCurrent:
                return setCurrentScene(scene);
            case PendingChange::Kind::Close:
                return closeScene(scene);
            case PendingChange::Kind::Remove:
                return removeScene(scene);
        }
        return false;
    }

    void SceneManager::tickCurrentScene(float timeDelta)
    {
        // Resets the flag even if a tick throws.
        struct TickScope
        {
            explicit TickScope(bool& flag)
                : flag(flag)
            {
                flag = true;
            }
            TickScope(const TickScope&) = delete;
            TickScope(TickScope&&) = delete;
            TickScope& operator=(const TickScope&) = delete;
            TickScope& operator=(TickScope&&) = delete;
            ~TickScope() { flag = false; }

            bool& flag;
        };

        const TickScope scope(_isTickingScene);
        _currentScene->tick(timeDelta);
    }

    bool SceneManager::rejectWhileTicking(const char* operation) const
    {
        if (!_isTickingScene)
        {
            return false;
        }
        errorLog(
            "Cannot {} while the current scene is ticking. Use the request*Scene() methods; "
            "they apply at the end of the frame."_f
            << operation);
        return true;
    }

    void SceneManager::resetScenes()
    {
        if (rejectWhileTicking("reset scenes"))
        {
            return;
        }
        std::vector<Core::IntrusivePtr<Scene>> replacement;
        auto scene = Core::IntrusivePtr<Scene>(new Scene());
        replacement.push_back(std::move(scene));
        _scenes.swap(replacement);
        _scenesOwningFile.clear();
        restoreOpenedScenes({ _scenes.front().get() }, _scenes.front().get());
    }

    spdlog::logger* SceneManager::getLogger() const
    {
        return NxWorld::getLogger();
    }

} // namespace NX
