// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "FreeTypeLibrary.h"

#include "../PrivateModuleInfo.h"

namespace RawBackend::Font
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

    Face FreeTypeLibrary::generateFace(const std::filesystem::path& path, FT_Long faceIndex)
    {
        Face out;
        FT_Face face = nullptr;
        if (auto err = FT_New_Face(ft, path.generic_string().c_str(), faceIndex, &face))
        {
            std::string msg = FT_Error_String(err);
            if (msg.empty())
            {
                msg = "<undefined>";
            }
            criticalLogAndAssert("Impossible to generate a FT_Face: " + msg);

            return {};
        }
        out._setFace(face);
        return out;
    }

    spdlog::logger* FreeTypeLibrary::getLogger() const
    {
        return RawBackend::getLogger();
    }

    FreeTypeLibrary::~FreeTypeLibrary()
    {
        destroy();
    }

    FreeTypeLibrary& GetFreeTypeLibrary()
    {
        return FreeTypeLibrary::Instance();
    }

    Face::Face(Face&& other) noexcept
    {
        release();
        swap(*this, other);
    }

    Face& Face::operator=(Face&& other) noexcept
    {
        if (this == &other) [[unlikely]]
        {
            return *this;
        }

        Face f(std::move(other));
        swap(*this, f);
        return *this;
    }

    void Face::_setFace(FT_Face face)
    {
        if (Verify(!_face.has_value()))
        {
            _face = face;
        }
    }

    void Face::release()
    {
        if (_face)
        {
            FT_Done_Face(*_face);
            _face = nullptr;
        }
    }
} // namespace RawBackend::Font