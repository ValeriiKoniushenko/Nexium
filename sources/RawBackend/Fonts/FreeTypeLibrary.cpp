// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "FreeTypeLibrary.h"

namespace RawBackend
{

    FreeTypeLibrary::FreeTypeLibrary()
    {
        init();
    }

    void FreeTypeLibrary::init()
    {
        if (FT_Init_FreeType(&ft))
        {
            throw std::runtime_error("Couldn't initialize freetype library");
        }
    }

    void FreeTypeLibrary::destroy()
    {
        FT_Done_FreeType(ft);
        ft = nullptr;
    }

    bool FreeTypeLibrary::generateFace(const std::filesystem::path& path, FT_Long faceIndex,
                                       FT_Face& face)
    {
        return FT_New_Face(ft, path.generic_string().c_str(), faceIndex, &face);
    }

    FreeTypeLibrary::~FreeTypeLibrary()
    {
        destroy();
    }

    FreeTypeLibrary& GetFreeTypeLibrary()
    {
        return FreeTypeLibrary::Instance();
    }
} // namespace RawBackend