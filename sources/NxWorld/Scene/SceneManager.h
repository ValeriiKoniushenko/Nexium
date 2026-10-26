// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "Core/Delegate.h"
#include "Core/String.h"
#include "NxFundamental/ResourceManagement/DataStream.h"
#include "Scene.h"

#include <memory>
#include <unordered_set>
#include <vector>

namespace NX
{
    class SceneManager final : public Foundation::BaseLog
    {
    public:
        // `world` receives the current scene's camera; it may be null when nothing renders.
        explicit SceneManager(World* world = nullptr);
        SceneManager(const SceneManager&) = delete;
        SceneManager(SceneManager&&) = delete;
        SceneManager& operator=(const SceneManager&) = delete;
        SceneManager& operator=(SceneManager&&) = delete;
        ~SceneManager() override;

        Scene& createNewScene();
        bool renameScene(Scene* scene, const Core::StringAtom& newName);

        // Immediate changes. They are refused while the current scene is ticking, because they
        // can deinitialize or destroy the scene that is running; game code uses the requests.
        bool setCurrentScene(const Core::StringAtom& sceneName);
        bool setCurrentScene(Scene* scene);
        bool closeScene(Scene* scene);

        // Deferred changes, applied in request order by applyPendingChanges() at the end of
        // the frame. Safe to call from any object's tick.
        void requestCurrentScene(const Core::StringAtom& sceneName);
        void requestCurrentScene(Scene* scene);
        void requestCloseScene(Scene* scene);
        void requestRemoveScene(Scene* scene);
        void applyPendingChanges();
        [[nodiscard]] bool hasPendingChanges() const noexcept { return !_pendingChanges.empty(); }

        void tickCurrentScene(float timeDelta);

        [[nodiscard]] bool hasScene(const Scene* scene) const;
        [[nodiscard]] bool isSceneOpen(const Scene* scene) const;
        [[nodiscard]] const std::vector<Scene*>& getOpenScenes() const noexcept
        {
            return _openScenes;
        }

        // Makes `opened` the open scenes and `current` (one of them) the current scene; scenes
        // that are no longer open are deinitialized.
        void restoreOpenedScenes(std::vector<Scene*> opened, Scene* current);
        // Opens only the world's start scene, or the first scene when it is unset or unknown.
        void openStartScene();
        // Loads every valid scene file. Broken files and duplicate names are logged, skipped
        // and left untouched on disk.
        void importScenes(const std::filesystem::path& directory);
        void readFromCache();
        bool writeToCache(CacheWriteMode mode = CacheWriteMode::Automatic);
        [[nodiscard]] bool hasImportError() const noexcept { return _importFailed; }
        [[nodiscard]] const std::vector<std::filesystem::path>& getSkippedSceneFiles()
            const noexcept
        {
            return _skippedSceneFiles;
        }

        void resetScenes();
        bool removeScene(const StringAtom& sceneName);
        bool removeScene(Scene* scene);

        [[nodiscard]] const std::vector<Core::IntrusivePtr<Scene>>& getScenes() const noexcept
        {
            return _scenes;
        }

        [[nodiscard]] Scene* getCurrentScene() const noexcept { return _currentScene; }
        [[nodiscard]] Scene* getScene(const Core::StringAtom& sceneName) const;

        // Fired after the current scene changes, before the previous scene is destroyed.
        Delegate<void(Scene*)>::Ptr onCurrentSceneChanged = Delegate<void(Scene*)>::Create();

        [[nodiscard]] spdlog::logger* getLogger() const override;

    private:
        struct PendingChange
        {
            enum class Kind
            {
                SetCurrent,
                Close,
                Remove
            };

            Kind kind;
            // Keeps the scene alive until the change is applied; the name is used when set.
            Core::IntrusivePtr<Scene> scene;
            Core::StringAtom sceneName;
        };

        [[nodiscard]] bool rejectWhileTicking(const char* operation) const;
        bool applyPendingChange(const PendingChange& change);
        [[nodiscard]] bool isSceneFileTaken(const Scene& scene, const Core::StringAtom& name) const;
        bool writeSceneFile(const Scene& scene, CacheWriteMode mode = CacheWriteMode::Automatic);
        void clearSceneFile(const Scene& scene);

        std::vector<Core::IntrusivePtr<Scene>> _scenes{};
        // Scenes whose file was loaded or written by this manager. Only they may replace or
        // delete their file, so a scene file that failed to import is never touched implicitly.
        std::unordered_set<const Scene*> _scenesOwningFile{};
        std::vector<std::filesystem::path> _skippedSceneFiles{};
        std::vector<Scene*> _openScenes{};
        World* _world = nullptr;
        Scene* _currentScene = nullptr;
        std::vector<PendingChange> _pendingChanges{};
        bool _importFailed = false;
        bool _isTickingScene = false;
    };

} // namespace NX
