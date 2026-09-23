// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "SceneManager.h"

#include <algorithm>
#include <fstream>
#include <stdexcept>

namespace NX
{
    nlohmann::json SceneManager::serialize() const
    {
        auto data = nlohmann::json{ { "formatVersion", 1 },
                                    { "scenes", nlohmann::json::array() },
                                    { "open", nlohmann::json::array() },
                                    { "current", 0 } };
        for (std::size_t i = 0; i < _scenes.size(); ++i)
        {
            data["scenes"].push_back(_scenes[i]->serialize());
            if (_scenes[i].get() == _currentScene)
            {
                data["current"] = i;
            }
        }
        for (const auto* scene : _openScenes)
        {
            const auto it = std::ranges::find_if(_scenes, [scene](const auto& owned)
                                                 { return owned.get() == scene; });
            data["open"].push_back(it - _scenes.begin());
        }
        return data;
    }

    void SceneManager::deserialize(RResourceStream<RJsonResourceStream>& stream)
    {
        const auto& data = stream.getData();
        if (data.at("formatVersion") != 1 || !data.at("scenes").is_array()
            || data.at("scenes").empty() || !data.at("open").is_array())
        {
            throw std::runtime_error("Invalid scene library format.");
        }
        std::vector<std::unique_ptr<Scene>> replacement;
        for (const auto& entry : data.at("scenes"))
        {
            auto scene = std::make_unique<Scene>();
            auto sceneData = RResourceStream<RJsonResourceStream>(entry);
            scene->deserialize(sceneData);
            scene->initialize();
            replacement.push_back(std::move(scene));
        }
        const auto current = data.at("current").get<std::size_t>();
        auto* selected = replacement.at(current).get();
        std::vector<Scene*> opened;
        for (const auto& index : data.at("open"))
        {
            auto* scene = replacement.at(index.get<std::size_t>()).get();
            if (std::ranges::find(opened, scene) != opened.end())
            {
                throw std::runtime_error("Duplicate open scene in library.");
            }
            opened.push_back(scene);
        }
        if (std::ranges::find(opened, selected) == opened.end())
        {
            throw std::runtime_error("Current scene must have an open tab.");
        }
        _scenes.swap(replacement);
        _openScenes = std::move(opened);
        _currentScene = selected;
        onCurrentSceneChanged->trigger(_currentScene);
    }

    void SceneManager::importScenes(const std::filesystem::path& directory)
    {
        if (!std::filesystem::exists(directory))
        {
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
        auto data = nlohmann::json{ { "formatVersion", 1 },
                                    { "scenes", nlohmann::json::array() },
                                    { "open", { 0 } },
                                    { "current", 0 } };
        for (const auto& path : paths)
        {
            std::ifstream file(path);
            data["scenes"].push_back(nlohmann::json::parse(file));
        }
        if (!paths.empty())
        {
            auto stream = RResourceStream<RJsonResourceStream>(data);
            deserialize(stream);
        }
    }
} // namespace NX
