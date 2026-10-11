// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

// Module: Nexium::World

#pragma once

#include "Core/Color.h"
#include "Foundation/BaseLog.h"
#include "Foundation/Interfaces/DataStream.h"
#include "GameState.h"
#include "JustReflectMe/Adapter.h"
#include "LevelData.h"
#include "PlayerState.h"

namespace NX
{
    class BaseCamera;

    CLASS();
    struct LightningProps : public Foundation::IDataIO
    {
        R_FRIEND(LightningProps);

        FIELD(R::Min = 0.0, R::Max = 10.0, R::Step = 0.05);
        float ambientStrength = 1.f;

        FIELD(R::Min = 0.0, R::Max = 10.0, R::Step = 0.05);
        float minLightStrength = 0.2f;

        FIELD(R::Min = 0.0, R::Max = 10.0, R::Step = 0.05);
        float specularStrength = 0.5f;

        FIELD(R::Min = 0.01, R::Max = 4096.0, R::Step = 2.0);
        float specularPow = 32.f;

        FIELD(R::Min = -1.0, R::Max = 1.0, R::Step = 0.01, R::SuppressLimits);
        glm::vec3 sunDirection = glm::vec3(0.5f);

        FIELD();
        Core::NormColor3 color = Core::NormColor3(1.f);

        [[nodiscard]] std::filesystem::path getCacheDir() const override;
        [[nodiscard]] Core::StringAtom getCacheHash() const override;
    };

    CLASS();
    class World : public Foundation::BaseLog, public Foundation::IDataIO
    {
        R_FRIEND(World);

    public:
        FIELD();
        Core::StringAtom worldName = "Default";

        LightningProps lightning;
        PlayerState playerState;
        GameState gameState;
        LevelData levelData;
        // It a reference to the real camera. It doesn't own it.
        BaseCamera* currentCamera = nullptr;

    public:
        [[nodiscard]] std::filesystem::path getCacheDir() const override;
        [[nodiscard]] Core::StringAtom getCacheHash() const override;
        [[nodiscard]] spdlog::logger* getLogger() const override;
        [[nodiscard]] const char* getPrefix() const override { return "World"; }
        [[nodiscard]] float getWorldTime() const noexcept { return _activeTime; }

        [[nodiscard]] float getTimeDelta() const noexcept { return _timeDelta; }
        void internal_UpdateTimeDelta(float delta) noexcept;

    private:
        float _timeDelta = 0.f;

        // How long the application(world) is active in seconds
        float _activeTime = 0.f;
    };

} // namespace NX

#include "World.generated.h" // added by the code generator. Better don't move it.
