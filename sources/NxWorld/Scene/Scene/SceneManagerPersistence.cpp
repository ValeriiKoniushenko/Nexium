// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "Foundation/Configs.h"
#include "Foundation/Debug/Latency.h"
#include "SceneManager.h"
#include "SceneTabsState.h"

#include <algorithm>
#include <fstream>
#include <iterator>
#include <stdexcept>

namespace NX
{
    nlohmann::json SceneManager::serialize() const
    {
        SceneTabsState state;
        state.current = _currentScene->getSceneName();
        for (const auto* scene : _openScenes)
        {
            state.open.push_back(scene->getSceneName());
        }
        return R<SceneTabsState>::Serialize(state).getData();
    }

    void SceneManager::deserialize(const nlohmann::json& data)
    {
        if (!data.at("formatVersion").is_number_integer() || data.at("formatVersion") != 1
            || !data.at("open").is_array())
        {
            throw std::runtime_error("Invalid scene tabs format.");
        }
        SceneTabsState state;
        const auto stream = RResourceStream<RJsonResourceStream>(data);
        R<SceneTabsState>::Deserialize(stream, state);
        auto* selected = getScene(state.current);
        std::vector<Scene*> opened;
        for (const auto& name : state.open)
        {
            auto* scene = getScene(name);
            if (scene && std::ranges::find(opened, scene) == opened.end())
            {
                opened.push_back(scene);
            }
        }
        if (!selected)
        {
            selected = opened.empty() ? _scenes.front().get() : opened.front();
        }
        if (std::ranges::find(opened, selected) == opened.end())
        {
            opened.push_back(selected);
        }
        restoreOpenedScenes(std::move(opened), selected);
    }

    void SceneManager::importScenes(const std::filesystem::path& directory)
    try
    {
        NX_LATENCY_POINT("Scene import started");
        _importFailed = false;
        _skippedSceneFiles.clear();
        if (!std::filesystem::exists(directory))
        {
            NX_LATENCY_POINT("Scene import finished");
            return;
        }
        std::vector<std::filesystem::path> paths;
        for (const auto& entry : std::filesystem::directory_iterator(directory))
        {
            if (entry.is_regular_file() && entry.path().extension() == ".json")
            {
                paths.push_back(entry.path());
            }
        }
        std::ranges::sort(paths);
        std::vector<Core::IntrusivePtr<Scene>> replacement;
        std::vector<std::filesystem::path> sources;
        for (const auto& path : paths)
        {
            try
            {
                std::ifstream file(path);
                auto stream = RResourceStream<RJsonResourceStream>(nlohmann::json::parse(file));
                auto scene = Core::IntrusivePtr<Scene>(new Scene());
                scene->deserialize(stream);
                const auto name = scene->getSceneName();
                if (name.isEmpty())
                {
                    throw std::runtime_error("Empty scene name.");
                }
                const auto duplicate = std::ranges::find_if(
                    replacement,
                    [&name](const auto& existing) { return existing->getSceneName() == name; });
                if (duplicate != replacement.end())
                {
                    const auto& firstSource = sources[static_cast<std::size_t>(
                        std::distance(replacement.begin(), duplicate))];
                    throw std::runtime_error("Duplicate scene name '" + name.toStdString()
                                             + "', already loaded from: "
                                             + firstSource.generic_string());
                }
                replacement.push_back(std::move(scene));
                sources.push_back(path);
            }
            catch (const std::exception& error)
            {
                _importFailed = true;
                _skippedSceneFiles.push_back(path);
                errorLog("Skipped scene file {}: {}"_f << path.generic_string() << error.what());
            }
        }
        if (!replacement.empty())
        {
            _scenes.swap(replacement);
            _scenesOwningFile.clear();
            for (std::size_t i = 0; i < _scenes.size(); ++i)
            {
                // A scene owns only the file it was read from; another file at its save path
                // stays untouched.
                std::error_code error;
                if (std::filesystem::equivalent(sources[i], GetCacheSystem().getPath(*_scenes[i]),
                                                error))
                {
                    _scenesOwningFile.insert(_scenes[i].get());
                }
            }
            restoreOpenedScenes({ _scenes.front().get() }, _scenes.front().get());
        }
        NX_LATENCY_POINT("Scene import finished");
    }
    catch (...)
    {
        _importFailed = true;
        throw;
    }

    void SceneManager::readFromCache()
    {
        try
        {
            importScenes(Foundation::Config::Path::data / "scenes");
            GetCacheSystem().tryRead(*this);
        }
        catch (const std::exception& error)
        {
            errorLog("Cannot load scene files: {}"_f << error.what());
        }
    }

    bool SceneManager::writeToCache(CacheWriteMode mode)
    {
        NX_LATENCY_POINT("Scene library save started");
        bool saved = true;
        for (const auto& scene : _scenes)
        {
            saved = writeSceneFile(*scene, mode) && saved;
        }
        saved = GetCacheSystem().write(*this, mode) && saved;
        NX_LATENCY_POINT("Scene library save finished");
        return saved;
    }
} // namespace NX
