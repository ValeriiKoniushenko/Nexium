// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "World.h"

#include "NxWorld/PrivateModuleInfo.h"

namespace NX
{

    std::filesystem::path LightningProps::getCacheDir() const
    {
        return "LightningProps";
    }

    Core::StringAtom LightningProps::getCacheHash() const
    {
        return "LightningProps";
    }

    spdlog::logger* World::getLogger() const
    {
        return NxWorld::getLogger();
    }

    void World::internal_UpdateTimeDelta(float delta) noexcept
    {
        _timeDelta = delta;
        _activeTime += delta;
    }

    std::filesystem::path World::getCacheDir() const
    {
        return "world";
    }

    Core::StringAtom World::getCacheHash() const
    {
        return worldName;
    }

} // namespace NX
