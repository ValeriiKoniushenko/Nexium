// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "../Image.h"
#include "Platform/Graphics.h"

#include <Core/IntrusivePtr.h>
#include <filesystem>

namespace RawBackend
{
    class Texture : public Core::IntrusiveRefCounter<Texture>
    {
        INTRUSIVE_PTR_ADAPTERS(Texture);

    public:
        Texture() = default;
        Texture(const Texture&) = delete;
        Texture& operator=(const Texture&) = delete;
        Texture(Texture&&) noexcept;
        Texture& operator=(Texture&&) noexcept;
        ~Texture() override;

        bool loadFromFile(const std::filesystem::path& path, bool isFlipVertically = true);
        void loadFromImage(const Image& data);

        void activateTextureUnit(GLuint unit) const;
        void generate();
        void bind() const;
        void unbind() const;
        void putImage(GLint level, GLint internalformat, GLsizei width, GLsizei height,
                      GLint border, GLenum format, GLenum type, const void* pixels);

        void putSubImage(GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height,
                         GLenum format, GLenum type, const void* pixels);

        void generateMipmap(GLint min = GL_NEAREST, GLint mag = GL_NEAREST,
                            GLint wrapS = GL_CLAMP_TO_EDGE, GLint wrapT = GL_CLAMP_TO_EDGE);

        [[nodiscard]] GLuint getTextureId() noexcept { return _textureId; }

        [[nodiscard]] bool isValid() const noexcept { return _textureId != 0; }
        [[nodiscard]] Core::ISize2 getSize() const noexcept { return _size; }

        void release();

        friend void swap(Texture& first, Texture& second) noexcept
        {
            std::swap(first._textureId, second._textureId);
            std::swap(first._size, second._size);
        }

    protected:
        GLuint _textureId = 0;
        Core::ISize2 _size;
    };
} // namespace RawBackend
