// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

// Module: Nexium::Subsystem::Graphics

#pragma once

#include "Core/IntrusivePtr.h"
#include "Foundation/BaseLog.h"
#include "Foundation/BaseUsings.h"

#include <filesystem>
#include <memory>

namespace RawBackend::Font
{
    class Face;
}

namespace NX
{
    /// How do Fonts work:
    /// It's going from the lowest abstraction layer: RawBackend::Font
    /// This layer (RawBackend) describes the minimum to be able to work with Font data:
    /// freetype (in such a case) initialization, collaborating with it to get font
    /// data - everything like this is. That layer promise only one thing: use some
    /// popular approach and adjust it to use in the scope of this code base. It's more
    /// like an adapter between freetype <-> and the Engine code.
    ///
    /// But, this exact module is being built on top of the previous one.
    /// The goal of it is to lock the prev impl, make it private(not impl), and provide new
    /// approaches to work with font in much more correct, convenient and safe way.
    /// So, due to it's a Graphics module and any fonts are about 'rendering' - the next
    /// code will bake the logic: "How to render a text XYZ using a font ZYX".

    /// @brief This class describes one specific Face. Taking into example the whole
    /// 'Roboto' family this class will describe only Robot-Medium-Regular
    /// Also, this class provide all needed sizes for future rendering and placing of
    /// the text on the render scene: glyph offsets, glyph sizes, etc.
    /// And with sizes, offsets it provides a ready-to-render Texture for future rendering.
    class Face : public Foundation::BaseLog, public Core::IntrusiveRefCounter<Face>
    {
    public:
        struct Glyph
        {
            Core::UISize2 size;   // Size of glyph
            Core::ISize2 bearing; // Offset from baseline to left/top of glyph
            int advance = 0;      // Offset to advance to next glyph
        };

    public:
        Face();
        ~Face() override = default;
        Face(const Face&) = delete;
        Face& operator=(const Face&) = delete;
        Face(Face&&) noexcept;
        Face& operator=(Face&&) noexcept;

        void setSourcePath(const std::filesystem::path& path);

        /// Before call of this function the source path must be set.
        [[nodiscard]] std::size_t getFacesNumber();

        /// Before call of this function the source path must be set.
        bool generateFace(std::size_t faceIndex);

        bool setPixelSize(Core::UISize2 size);
        bool setPixelSize(std::size_t height);

        void release();

        friend void swap(Face& a, Face& b) noexcept { std::swap(a._face, b._face); }

        [[nodiscard]] const char* getPrefix() const override { return "Face"; }
        [[nodiscard]] spdlog::logger* getLogger() const override;

    protected:
        std::unique_ptr<RawBackend::Font::Face> _face;
    };

} // namespace NX