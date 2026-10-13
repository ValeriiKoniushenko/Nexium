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

#include "Core/IntrusivePtr.h"
#include "Core/Singleton.h"
#include "Foundation/BaseLog.h"
#include "Foundation/BaseUsings.h"
#include "ft2build.h"

#include <cstdint>
#include FT_FREETYPE_H

#include <filesystem>

namespace RawBackend::Font
{

    /// It supports only variable fonts.
    class Face : public Foundation::BaseLog, public Core::IntrusiveRefCounter<Face>
    {
    public:
        constexpr static FT_Long NoFaces = -1;

    public:
        Face() = default;
        ~Face() override = default;
        Face(const Face&) = delete;
        Face(Face&& other) noexcept;
        Face& operator=(const Face&) = delete;
        Face& operator=(Face&& other) noexcept;

        [[nodiscard]] bool isValid() const noexcept { return _face != nullptr; }

        friend void swap(Face& a, Face& b) noexcept
        {
            std::swap(a._path, b._path);
            std::swap(a._face, b._face);
            std::swap(a._faceNumber, b._faceNumber);
        }

        void setSourcePath(const std::filesystem::path& path);
        [[nodiscard]] FT_Long getFacesNumber();
        bool generateFace(FT_Long faceIndex);
        bool setPixelSize(Core::UISize2 size);
        bool setPixelSize(unsigned int height);
        void release();

        /// Return non-owning pointer to a Glyph. This glyph will be released after next load.
        [[nodiscard]] FT_GlyphSlot loadChar(FT_ULong charCode, FT_Int32 loadFlags);

        [[nodiscard]] const char* getPrefix() const override { return "Face"; }
        [[nodiscard]] spdlog::logger* getLogger() const override;

    private:
        void probe();

    private:
        // Don't use std::filesystem::path to speed up work & data storing
        std::string _path;
        FT_Face _face = nullptr;
        FT_Long _faceNumber = NoFaces;
    };

    class FreeTypeLibrary : public Foundation::BaseLog, public Core::Singleton<FreeTypeLibrary>
    {
        SINGLETONS_FRIEND_NO_CNSTR(FreeTypeLibrary);

    public:
        FreeTypeLibrary(const FreeTypeLibrary&) = delete;
        FreeTypeLibrary(FreeTypeLibrary&&) = delete;
        FreeTypeLibrary& operator=(const FreeTypeLibrary&) = delete;
        FreeTypeLibrary& operator=(FreeTypeLibrary&&) = delete;

        FreeTypeLibrary();
        ~FreeTypeLibrary() override;

        [[nodiscard]] const char* getPrefix() const override { return "FontBackend"; }
        [[nodiscard]] spdlog::logger* getLogger() const override;

        [[nodiscard]] std::string getLastFtErrorString(FT_Error err) const;
        [[nodiscard]] FT_Face generateFace(const std::filesystem::path& path, FT_Long faceIndex);

    private:
        void init();
        void destroy();

    private:
        FT_Library ft = nullptr;
    };

    [[nodiscard]] FreeTypeLibrary& GetFreeTypeLibrary();

} // namespace RawBackend::Font