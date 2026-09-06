/*
 * MIT License
 *
 * Copyright (c) 2018-2027 Valerii Koniushenko
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

#include "Texture.h"

namespace Core
{
    Texture::Texture(Texture&& other) noexcept
        : _textureId(other._textureId),
          _size(other._size)
    {
        other._textureId = 0;
        other._size = {};
    }

    Texture& Texture::operator=(Texture&& other) noexcept
    {
        if (&other == this) [[unlikely]]
        {
            return *this;
        }

        Texture temp(std::move(other));
        swap(*this, temp);
        return *this;
    }

    Texture::~Texture()
    {
        release();
    }

    bool Texture::loadFromFile(const std::filesystem::path& path, bool isFlipVertically)
    {
        Image img;
        if (!img.loadFromFile(path, isFlipVertically))
        {
            return false;
        }

        _size = img.getSize();
        if (!isValid())
        {
            generate();
        }

        bind();

        putImage(0, GL_RGBA, img.getSize().width, img.getSize().height, 0,
                 img.getChannelAsOpenGLType(), GL_UNSIGNED_BYTE, img.data());

        generateMipmap(GL_LINEAR, GL_LINEAR);

        return true;
    }

    void Texture::loadFromImage(const Image& data)
    {
        _size = data.getSize();

        generate();
        bind();

        putImage(0, GL_RGBA, data.getSize().width, data.getSize().height, 0,
                 data.getChannelAsOpenGLType(), GL_UNSIGNED_BYTE, data.data());

        generateMipmap(GL_LINEAR, GL_LINEAR);
        unbind();
    }

    void Texture::release()
    {
        if (_size.height != 0 && _textureId != 0)
        {
            glDeleteTextures(1, &_textureId);
            _textureId = 0;
            _size = {};
        }
    }

    void Texture::activateTextureUnit(GLuint unit) const
    {
        glActiveTexture(GL_TEXTURE0 + unit);
    }

    void Texture::generate()
    {
        if (isValid()) [[unlikely]]
        {
            release();
            Assert(
                false,
                "Texture is already generated. You should release it before generating a new one.");
            return;
        }

        glGenTextures(1, &_textureId);
    }

    void Texture::bind() const
    {
        if (!isValid()) [[unlikely]]
        {
            Assert(false, "Texture is not generated. You should generate it before binding.");
            return;
        }

        glBindTexture(GL_TEXTURE_2D, _textureId);
    }

    void Texture::unbind() const
    {
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void Texture::putImage(GLint level, GLint internalformat, GLsizei width, GLsizei height,
                           GLint border, GLenum format, GLenum type, const void* pixels)
    {
        if (!isValid()) [[unlikely]]
        {
            Assert(false,
                   "Texture is not generated. You should generate it before putting image data.");
            return;
        }

        glTexImage2D(GL_TEXTURE_2D, level, internalformat, width, height, border, format, type,
                     pixels);
    }

    void Texture::putSubImage(GLint level, GLint xoffset, GLint yoffset, GLsizei width,
                              GLsizei height, GLenum format, GLenum type, const void* pixels)
    {
        glTexSubImage2D(GL_TEXTURE_2D, level, xoffset, yoffset, width, height, format, type,
                        pixels);
    }

    void Texture::generateMipmap(GLint min, GLint mag, GLint wrapS, GLint wrapT)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);
        glGenerateMipmap(GL_TEXTURE_2D);
    }

} // namespace Core