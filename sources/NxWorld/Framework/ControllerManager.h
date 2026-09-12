// Nexium
// Copyright 2018-2027 Valerii Koniushenko
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
    class ControllerManager : public Foundation::BaseLog
    {
    public:
        ControllerManager() = default;
        ~ControllerManager() override = default;
        ControllerManager(const ControllerManager&) = delete;
        ControllerManager(ControllerManager&&) noexcept = delete;
        ControllerManager& operator=(const ControllerManager&) = delete;
        ControllerManager& operator=(ControllerManager&&) noexcept = delete;

        [[nodiscard]] spdlog::logger* getLogger() const override;
        [[nodiscard]] const char* getPrefix() const override { return "ControllerManager"; }
    };
} // namespace Core
