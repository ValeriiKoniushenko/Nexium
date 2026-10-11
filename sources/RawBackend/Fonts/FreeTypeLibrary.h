// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

// Module: Nexium::RawBackend::Graphics

#pragma once

#include "Core/Singleton.h"
#include "ft2build.h"
#include FT_FREETYPE_H

#include <filesystem>

namespace RawBackend
{
    class FreeTypeLibrary : public Core::Singleton<FreeTypeLibrary>
    {
        SINGLETONS_FRIEND_NO_CNSTR(FreeTypeLibrary);

    public:
        FreeTypeLibrary(const FreeTypeLibrary&) = delete;
        FreeTypeLibrary(FreeTypeLibrary&&) = delete;
        FreeTypeLibrary& operator=(const FreeTypeLibrary&) = delete;
        FreeTypeLibrary& operator=(FreeTypeLibrary&&) = delete;

        FreeTypeLibrary();
        ~FreeTypeLibrary() override;

        bool generateFace(const std::filesystem::path& path, FT_Long faceIndex, FT_Face& face);

    private:
        void init();
        void destroy();

    private:
        FT_Library ft = nullptr;
    };

    [[nodiscard]] FreeTypeLibrary& GetFreeTypeLibrary();

} // namespace RawBackend