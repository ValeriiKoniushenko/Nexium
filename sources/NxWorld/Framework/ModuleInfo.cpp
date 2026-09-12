// Nexium
// Copyright 2018-2027 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "Foundation/BaseLog.h"
#include "PrivateModuleInfo.h"
#include "spdlog/sinks/stdout_color_sinks.h"

namespace Framework
{

    spdlog::logger* getLogger()
    {
        static auto logger = spdlog::stdout_color_mt("Framework");
        return logger.get();
    }

} // namespace Framework