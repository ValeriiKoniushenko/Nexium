// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

// Module: Nexium::Editor

#pragma once

#include "Foundation/BaseLog.h"
#include "NxFundamental/ResourceManagement/DataStream.h"

namespace NX
{
    class SceneManager;

    // Editor-only state: which scenes are open as tabs and which one is selected. The game itself
    // starts from World::startScene and never reads this.
    class SceneTabsCache final : public Foundation::BaseLog, public Foundation::IDataIO
    {
    public:
        explicit SceneTabsCache(SceneManager& scenes);

        [[nodiscard]] nlohmann::json serialize() const;
        void deserialize(const nlohmann::json& data);
        void deserialize(RResourceStream<RJsonResourceStream>& stream)
        {
            deserialize(stream.getData());
        }

        [[nodiscard]] std::filesystem::path getCacheDir() const override { return "cache"; }
        [[nodiscard]] StringAtom getCacheHash() const override { return "SceneTabs"_atom; }
        [[nodiscard]] spdlog::logger* getLogger() const override;

    private:
        SceneManager& _scenes;
    };
} // namespace NX
