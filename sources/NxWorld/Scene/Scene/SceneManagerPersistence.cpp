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
                                    { "open", nlohmann::json::array() },
                                    { "current", _currentScene->getSceneName().c_str() } };
        for (const auto* scene : _openScenes)
        {
            data["open"].push_back(scene->getSceneName().c_str());
        }
        return data;
    }

    void SceneManager::deserialize(RResourceStream<RJsonResourceStream>& stream)
    {
        const auto& data = stream.getData();
        if (data.at("formatVersion") != 1 || !data.at("open").is_array())
        {
            throw std::runtime_error("Invalid scene tabs format.");
        }
        auto* selected = getScene(Core::StringAtom::Intern(data.at("current").get<std::string>()));
        std::vector<Scene*> opened;
        for (const auto& name : data.at("open"))
        {
            auto* scene = getScene(Core::StringAtom::Intern(name.get<std::string>()));
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
        std::vector<std::unique_ptr<Scene>> replacement;
        for (const auto& path : paths)
        {
            std::ifstream file(path);
            auto stream = RResourceStream<RJsonResourceStream>(nlohmann::json::parse(file));
            auto scene = std::make_unique<Scene>();
            scene->deserialize(stream);
            const auto name = scene->getSceneName();
            if (name.isEmpty()
                || std::ranges::any_of(replacement, [&name](const auto& existing)
                                       { return existing->getSceneName() == name; }))
            {
                throw std::runtime_error("Empty or duplicate scene name.");
            }
            scene->initialize();
            replacement.push_back(std::move(scene));
        }
        if (!replacement.empty())
        {
            _scenes.swap(replacement);
            _currentScene = _scenes.front().get();
            _openScenes = { _currentScene };
            onCurrentSceneChanged->trigger(_currentScene);
        }
    }
} // namespace NX
