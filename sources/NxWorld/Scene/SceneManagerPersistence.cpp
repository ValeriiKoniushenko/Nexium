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

#include <algorithm>
#include <fstream>
#include <iterator>
#include <stdexcept>

namespace NX
{
    void SceneManager::importScenes(const std::filesystem::path& directory)
    try
    {
        if (rejectWhileTicking("import scenes"))
        {
            return;
        }
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
                const auto duplicate
                    = std::ranges::find_if(replacement, [&name](const auto& existing)
                                           { return existing->getSceneName() == name; });
                if (duplicate == replacement.end())
                {
                    replacement.push_back(std::move(scene));
                    sources.push_back(path);
                    continue;
                }
                auto& loadedSource = sources[static_cast<std::size_t>(
                    std::distance(replacement.begin(), duplicate))];
                // A copied file can sort first; the file named after the scene still wins.
                if (path.filename() != GetCacheSystem().getPath(*scene).filename())
                {
                    throw std::runtime_error("Duplicate scene name '" + name.toStdString()
                                             + "', already loaded from: "
                                             + loadedSource.generic_string());
                }
                _importFailed = true;
                _skippedSceneFiles.push_back(loadedSource);
                errorLog(
                    "Skipped scene file {}: Duplicate scene name '{}', loaded from {} instead."_f
                    << loadedSource.generic_string() << name << path.generic_string());
                *duplicate = std::move(scene);
                loadedSource = path;
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
        NX_LATENCY_POINT("Scene library save finished");
        return saved;
    }
} // namespace NX
