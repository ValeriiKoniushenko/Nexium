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
    class SceneManager final : public Foundation::BaseLog, public Foundation::IDataIO
    {
    public:
        SceneManager();
        SceneManager(const SceneManager&) = delete;
        SceneManager(SceneManager&&) = delete;
        SceneManager& operator=(const SceneManager&) = delete;
        SceneManager& operator=(SceneManager&&) = delete;
        ~SceneManager() override;

        Scene& createNewScene();
        bool renameScene(Scene* scene, const Core::StringAtom& newName);
        bool setCurrentScene(const Core::StringAtom& sceneName);
        bool setCurrentScene(Scene* scene);

        bool closeScene(Scene* scene);
        [[nodiscard]] bool hasScene(const Scene* scene) const;
        [[nodiscard]] bool isSceneOpen(const Scene* scene) const;
        [[nodiscard]] const std::vector<Scene*>& getOpenScenes() const noexcept
        {
            return _openScenes;
        }

        [[nodiscard]] nlohmann::json serialize() const;
        void deserialize(const nlohmann::json& data);
        void deserialize(RResourceStream<RJsonResourceStream>& stream)
        {
            deserialize(stream.getData());
        }
        // Loads every valid scene file. Broken files and duplicate names are logged, skipped
        // and left untouched on disk.
        void importScenes(const std::filesystem::path& directory);
        void readFromCache();
        bool writeToCache(CacheWriteMode mode = CacheWriteMode::Automatic);
        [[nodiscard]] bool hasImportError() const noexcept { return _importFailed; }
        [[nodiscard]] const std::vector<std::filesystem::path>& getSkippedSceneFiles() const noexcept
        {
            return _skippedSceneFiles;
        }

        [[nodiscard]] std::filesystem::path getCacheDir() const override { return "cache"; }
        [[nodiscard]] StringAtom getCacheHash() const override { return "SceneTabs"_atom; }

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
        void restoreOpenedScenes(std::vector<Scene*> opened, Scene* current);
        [[nodiscard]] bool isSceneFileTaken(const Scene& scene,
                                            const Core::StringAtom& name) const;
        bool writeSceneFile(const Scene& scene, CacheWriteMode mode = CacheWriteMode::Automatic);
        void clearSceneFile(const Scene& scene);

        std::vector<Core::IntrusivePtr<Scene>> _scenes{};
        // Scenes whose file was loaded or written by this manager. Only they may replace or
        // delete their file, so a scene file that failed to import is never touched implicitly.
        std::unordered_set<const Scene*> _scenesOwningFile{};
        std::vector<std::filesystem::path> _skippedSceneFiles{};
        std::vector<Scene*> _openScenes{};
        Scene* _currentScene = nullptr;
        bool _importFailed = false;
    };

} // namespace NX
