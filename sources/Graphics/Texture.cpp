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

#include "Texture.h"

namespace Core
{
    Texture::Texture(Texture&& other) noexcept
    {
        *this = std::move(other);
    }

    Texture& Texture::operator=(Texture&& other) noexcept
    {
        if (&other != this) [[likely]]
        {
            _textureId = other._textureId;
            _size = other._size;

            other._textureId = 0;
            other._size = {};
        }
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

        glGenTextures(1, &_textureId);
        glBindTexture(GL_TEXTURE_2D, _textureId);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.getSize().width, img.getSize().height, 0,
                     img.getChannelAsOpenGLType(), GL_UNSIGNED_BYTE, img.data());

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        return true;
    }

    void Texture::release()
    {
        glDeleteTextures(1, &_textureId);
        _textureId = 0;
        _size = {};
    }
} // namespace Core