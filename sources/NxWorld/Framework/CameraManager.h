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
    class CameraManager : public Foundation::BaseLog
    {
    public:
        CameraManager() = default;
        ~CameraManager() override = default;
        CameraManager(const CameraManager&) = delete;
        CameraManager(CameraManager&&) noexcept = delete;
        CameraManager& operator=(const CameraManager&) = delete;
        CameraManager& operator=(CameraManager&&) noexcept = delete;

        [[nodiscard]] spdlog::logger* getLogger() const override;
        [[nodiscard]] const char* getPrefix() const override { return "CameraManager"; }
    };
} // namespace Core
