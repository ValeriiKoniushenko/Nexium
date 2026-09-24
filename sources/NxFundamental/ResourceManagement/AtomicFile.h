// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

// Module: Nexium::Fundamental::ResourceManagement

#pragma once

#include <filesystem>
#include <string_view>

namespace NX
{
    // Writes a sibling temporary file, then replaces the destination. Throws on failure.
    void WriteFileAtomically(const std::filesystem::path& path, std::string_view contents);
} // namespace NX
