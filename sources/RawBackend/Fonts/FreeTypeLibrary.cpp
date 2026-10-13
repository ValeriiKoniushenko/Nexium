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
#include "freetype/freetype.h"

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

    spdlog::logger* FreeTypeLibrary::getLogger() const
    {
        return RawBackend::getLogger();
    }

    std::string FreeTypeLibrary::getLastFtErrorString(FT_Error err) const
    {
        const char* msg = FT_Error_String(err);
        if (!msg)
        {
            return {};
        }
        return { msg };
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

    void Face::setSourcePath(const std::filesystem::path& path)
    {
        release();
        _path = path.generic_string();
    }

    FT_Long Face::getFacesNumber()
    {
        probe();
        return _faceNumber;
    }

    bool Face::generateFace(FT_Long faceIndex)
    {
        if (_path.empty())
        {
            return false;
        }

        probe();
        if (faceIndex >= _faceNumber)
        {
            Assert(false);
            return false;
        }

        _face = FreeTypeLibrary::Instance().generateFace(_path, faceIndex);
        return !!_face;
    }

    void Face::release()
    {
        if (_face)
        {
            FT_Done_Face(_face);
            _face = nullptr;
        }
        _path.clear();
        _faceNumber = NoFaces;
    }

    void Face::probe()
    {
        if (!_path.empty() && _faceNumber == NoFaces)
        {
            if (auto* f = FreeTypeLibrary::Instance().generateFace(_path, -1))
            {
                _faceNumber = f->num_faces;
                FT_Done_Face(f);
            }
        }
    }

    FT_Face FreeTypeLibrary::generateFace(const std::filesystem::path& path, FT_Long faceIndex)
    {
        FT_Face face = nullptr;
        if (auto err = FT_New_Face(ft, path.generic_string().c_str(), faceIndex, &face))
        {
            std::string msg = getLastFtErrorString(err);
            if (msg.empty())
            {
                msg = "<undefined>";
            }

            criticalLogAndAssert("Impossible to generate a FT_Face: " + msg);
            return {};
        }
        return face;
    }

} // namespace RawBackend::Font