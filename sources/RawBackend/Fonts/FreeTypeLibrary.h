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
#include "ft2build.h"
#include FT_FREETYPE_H

#include <filesystem>

namespace RawBackend::Font
{
    class Face : Core::IntrusiveRefCounter<Face>
    {
    public:
        Face() = default;
        ~Face() override = default;
        Face(const Face&) = delete;
        Face(Face&& other) noexcept;
        Face& operator=(const Face&) = delete;
        Face& operator=(Face&& other) noexcept;

        [[nodiscard]] bool isValid() const noexcept { return _face.has_value(); }

        void _setFace(FT_Face face);

        friend void swap(Face& a, Face& b) noexcept { std::swap(a._face, b._face); }

        void release();

    private:
        std::optional<FT_Face> _face = nullptr;
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

        [[nodiscard]] Face generateFace(const std::filesystem::path& path, FT_Long faceIndex);

        [[nodiscard]] const char* getPrefix() const override { return "FontBackend"; }
        [[nodiscard]] spdlog::logger* getLogger() const override;

    private:
        void init();
        void destroy();

    private:
        FT_Library ft = nullptr;
    };

    [[nodiscard]] FreeTypeLibrary& GetFreeTypeLibrary();

} // namespace RawBackend::Font