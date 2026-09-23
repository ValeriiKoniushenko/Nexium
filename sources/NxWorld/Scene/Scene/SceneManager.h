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
#include "Scene.h"

#include <memory>
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
        ~SceneManager() override = default;

        Scene& createNewScene();
        bool renameScene(Scene* scene, Core::StringAtom newName);
        bool setCurrentScene(const Core::StringAtom& sceneName);
        bool setCurrentScene(Scene* scene);

        bool closeScene(Scene* scene);
        [[nodiscard]] bool isSceneOpen(const Scene* scene) const;
        [[nodiscard]] const std::vector<Scene*>& getOpenScenes() const noexcept
        {
            return _openScenes;
        }

        [[nodiscard]] nlohmann::json serialize() const;
        void deserialize(RResourceStream<RJsonResourceStream>& stream);
        void importScenes(const std::filesystem::path& directory);

        [[nodiscard]] std::filesystem::path getCacheDir() const override { return "scene-library"; }
        [[nodiscard]] StringAtom getCacheHash() const override { return "Library"_atom; }

        void removeAllScenes();
        void removeScene(StringAtom sceneName);
        void removeScene(Scene* scene);

        [[nodiscard]] const std::vector<std::unique_ptr<Scene>>& getScenes() const noexcept
        {
            return _scenes;
        }

        [[nodiscard]] Scene* getCurrentScene() const noexcept { return _currentScene; }
        [[nodiscard]] Scene* getScene(const Core::StringAtom& sceneName) const;

        // Fired after the current scene changes, before the previous scene is destroyed.
        Delegate<void(Scene*)>::Ptr onCurrentSceneChanged = Delegate<void(Scene*)>::Create();

        [[nodiscard]] spdlog::logger* getLogger() const override;

    private:
        std::vector<std::unique_ptr<Scene>> _scenes;
        std::vector<Scene*> _openScenes;
        Scene* _currentScene = nullptr;
    };

} // namespace NX
