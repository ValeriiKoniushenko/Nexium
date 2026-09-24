// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

// Module: Nexium::Fundamental::Utils

#pragma once

#include <string>
#include <string_view>

namespace NX::Stringify
{
    extern const std::string_view gcDefaultNoneString;

    [[nodiscard]] std::string GetNoneIfEmpty(std::string_view str);
} // namespace NX::Stringify
