// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "PrivateModuleInfo.h"

#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"

namespace RawBackend
{
    const std::string_view gModuleName = PRIVATE_MODULE_NAME;

    spdlog::logger* getLogger()
    {
        static auto logger = spdlog::stdout_color_mt(gModuleName.data());
        return logger.get();
    }

} // namespace RawBackend