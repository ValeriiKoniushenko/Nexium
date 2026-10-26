// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "SceneTabsCache.h"

#include "Editor/PrivateModuleInfo.h"
#include "NxWorld/Scene/SceneManager.h"
#include "SceneTabsState.h"

#include <algorithm>
#include <stdexcept>

namespace NX
{
    SceneTabsCache::SceneTabsCache(SceneManager& scenes)
        : _scenes(scenes)
    {
    }

    nlohmann::json SceneTabsCache::serialize() const
    {
        SceneTabsState state;
        state.current = _scenes.getCurrentScene()->getSceneName();
        for (const auto* scene : _scenes.getOpenScenes())
        {
            state.open.push_back(scene->getSceneName());
        }
        return R<SceneTabsState>::Serialize(state).getData();
    }

    void SceneTabsCache::deserialize(const nlohmann::json& data)
    {
        if (!data.at("formatVersion").is_number_integer() || data.at("formatVersion") != 1
            || !data.at("open").is_array())
        {
            throw std::runtime_error("Invalid scene tabs format.");
        }
        SceneTabsState state;
        const auto stream = RResourceStream<RJsonResourceStream>(data);
        R<SceneTabsState>::Deserialize(stream, state);
        // The start scene wins over the remembered tab, so the editor opens on the scene the game
        // starts with even when every scene tab is open.
        auto* selected = _scenes.findStartScene();
        if (!selected)
        {
            selected = _scenes.getScene(state.current);
        }
        std::vector<Scene*> opened;
        for (const auto& name : state.open)
        {
            auto* scene = _scenes.getScene(name);
            if (scene && std::ranges::find(opened, scene) == opened.end())
            {
                opened.push_back(scene);
            }
        }
        if (!selected)
        {
            selected = opened.empty() ? _scenes.getScenes().front().get() : opened.front();
        }
        if (std::ranges::find(opened, selected) == opened.end())
        {
            opened.push_back(selected);
        }
        _scenes.restoreOpenedScenes(std::move(opened), selected);
    }

    spdlog::logger* SceneTabsCache::getLogger() const
    {
        return Editor::getLogger();
    }
} // namespace NX
