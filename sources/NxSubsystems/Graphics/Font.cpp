// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "Font.h"

#include "NxSubsystems/Graphics/ShaderProgram.h"
#include "NxSubsystems/PrivateModuleInfo.h"
#include "RawBackend/Fonts/FreeTypeLibrary.h"

namespace NX
{
    Face::Face()
        : _face{ std::make_unique<RawBackend::Font::Face>() }
    {
    }

    Face::Face(Face&& other) noexcept
    {
        release();
        swap(*this, other);
    }

    Face& Face::operator=(Face&& other) noexcept
    {
        if (&other == this) [[unlikely]]
        {
            return *this;
        }

        Face tmp(std::move(*this));
        swap(other, *this);
        return *this;
    }

    void Face::setSourcePath(const std::filesystem::path& path)
    {
        _face->setSourcePath(path);
    }

    std::size_t Face::getFacesNumber()
    {
        return _face->getFacesNumber();
    }

    bool Face::generateFace(std::size_t faceIndex)
    {
        return _face->generateFace(static_cast<FT_Long>(faceIndex));
    }

    bool Face::setPixelSize(Core::UISize2 size)
    {
        return _face->setPixelSize(size);
    }

    bool Face::setPixelSize(std::size_t height)
    {
        return _face->setPixelSize(static_cast<unsigned int>(height));
    }

    bool Face::generateTextureASCIIGlyphs(ShaderProgram* shader)
    {
        if (!shader)
        {
            errorLogAndAssert("Impossible to generate a font '{}'. The passed shader is NULL."_f
                              << _face->getSourcePath());
            return false;
        }

        _glyphs.reserve(128);

        // TODO: move it to the OpenGL abstraction
        GLint prev{};
        glGetIntegerv(GL_UNPACK_ALIGNMENT, &prev);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        std::vector<unsigned char> missedGlyphs;
        for (unsigned char c = 0; static_cast<std::size_t>(c) < _glyphs.size(); ++c)
        {
            FT_GlyphSlot glyph = _face->loadChar(c, FT_LOAD_RENDER);
            if (!glyph)
            {
                missedGlyphs.push_back(c);
                continue;
            }

            _glyphs[c].glyph = GlyphSize{
                .size = Core::UISize2{ glyph->bitmap.width, glyph->bitmap.rows },
                .bearing = Core::ISize2{ glyph->bitmap_left, glyph->bitmap_top },
                .advance = static_cast<int>(glyph->advance.x),
            };

            auto&& rawChar = _glyphs[c].glyph;
            auto& rawText = _glyphs[c].texture;

            rawText.generate();
            rawText.bind();
            // clang-format off
            rawText.putImage(
                0,
                GL_RED,
                static_cast<GLsizei>(rawChar.size.width),
                static_cast<GLsizei>(rawChar.size.height),
                0,
                GL_RED,
                GL_UNSIGNED_BYTE,
                glyph->bitmap.buffer);
            // clang-format on
            rawText.generateMipmap(GL_LINEAR, GL_LINEAR);
        }
        _face->release();
        glPixelStorei(GL_UNPACK_ALIGNMENT, prev);

        // BaseGraphicsData g;
        // g.generate();
        // g.setShader(shader);
        // shader->callSetEvent(ShaderProgram::Event::OnSetVertexBuffer);

        return true;
    }

    void Face::release()
    {
        _face->release();
        _glyphs.clear();
    }

    spdlog::logger* Face::getLogger() const
    {
        return NxSubsystems::getLogger();
    }

} // namespace NX
