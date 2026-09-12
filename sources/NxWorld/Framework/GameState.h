// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "Foundation/BaseLog.h"

namespace Core
{
    class GameState : public Foundation::BaseLog
    {
    public:
        GameState() = default;
        ~GameState() override = default;
        GameState(const GameState&) = delete;
        GameState(GameState&&) noexcept = delete;
        GameState& operator=(const GameState&) = delete;
        GameState& operator=(GameState&&) noexcept = delete;

        [[nodiscard]] spdlog::logger* getLogger() const override;
        [[nodiscard]] const char* getPrefix() const override { return "GameState"; }
    };
} // namespace Core
