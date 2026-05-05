/*
 * MIT License
 *
 * Copyright (c) 2018-2025 Valerii Koniushenko
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#pragma once

#include "Image.h"
#include "OpenGL.h"

#include <filesystem>

namespace Core
{
    class Texture
    {
    public:
        Texture() = default;
        Texture(const Texture&) = delete;
        Texture& operator=(const Texture&) = delete;
        Texture(Texture&&) noexcept;
        Texture& operator=(Texture&&) noexcept;
        virtual ~Texture();

        bool loadFromFile(const std::filesystem::path& path, bool isFlipVertically = true);
        void loadFromImage(const Image& data);

        void generate();
        void bind() const;
        void unbind() const;
        void putImage(GLint level, GLint internalformat, GLsizei width, GLsizei height,
                      GLint border, GLenum format, GLenum type, const void* pixels) const;

        void putSubImage(GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height,
                         GLenum format, GLenum type, const void* pixels) const;

        void generateMipmap(GLint min = GL_NEAREST, GLint mag = GL_NEAREST,
                            GLint wrapS = GL_CLAMP_TO_EDGE, GLint wrapT = GL_CLAMP_TO_EDGE);

        [[nodiscard]] GLuint getTextureId() noexcept { return _textureId; }

        [[nodiscard]] bool isValid() const noexcept { return _textureId != 0; }
        [[nodiscard]] ISize2 getSize() const noexcept { return _size; }

        void release();

        friend void swap(Texture& first, Texture& second) noexcept
        {
            std::swap(first._textureId, second._textureId);
            std::swap(first._size, second._size);
        }

    protected:
        GLuint _textureId = 0;
        ISize2 _size;
    };
} // namespace Core
