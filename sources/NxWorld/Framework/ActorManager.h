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
    class ActorManager : public Foundation::BaseLog
    {
    public:
        ActorManager() = default;
        ~ActorManager() override = default;
        ActorManager(const ActorManager&) = delete;
        ActorManager(ActorManager&&) noexcept = delete;
        ActorManager& operator=(const ActorManager&) = delete;
        ActorManager& operator=(ActorManager&&) noexcept = delete;

        [[nodiscard]] spdlog::logger* getLogger() const override;
        [[nodiscard]] const char* getPrefix() const override { return "ActorManager"; }
    };
} // namespace Core
