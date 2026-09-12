// Nexium
// Copyright 2018-2027 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include <string_view>

namespace spdlog
{
    class logger;
}

namespace NxWorld
{

    extern const std::string_view gModuleName;

    [[nodiscard]] spdlog::logger* getLogger();

} // namespace NxWorld
