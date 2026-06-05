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

#include "Image.h"

#include "ModuleInfo.h"
#define STB_IMAGE_IMPLEMENTATION
#include "Stb/Image.h"

namespace Core
{
    Image::Image(const std::filesystem::path& path)
    {
        loadFromFile(path);
    }

    Image::~Image()
    {
        clear();
    }

    Image::Image(Image&& obj) noexcept
        : _path(std::move(obj._path)),
          _data(obj._data),
          _size(obj._size),
          _channel(obj._channel)
    {
        obj._data = nullptr;
        obj._size = {};
        obj._channel = Channel::None;
    }

    Image& Image::operator=(Image&& obj) noexcept
    {
        if (this == &obj) [[unlikely]]
        {
            return *this;
        }
        Image temp(std::move(obj));
        swap(*this, temp);
        return *this;
    }

    spdlog::logger* Image::getLogger() const
    {
        return Graphics::getLogger();
    }

    std::set<std::string> Image::SupportedExtensions()
    {
        return {
            // Read & Write
            ".png", ".jpg", ".jpeg", ".bmp", ".tga",
            ".gif", // read only (first frame)
            ".hdr",
            ".psd", // read only (composite)
            ".pic", // read only (Softimage PIC)
            ".pnm", // read only (.ppm, .pgm)
        };
    }

    GLenum Image::getChannelAsOpenGLType() const noexcept
    {
        if (static_cast<int>(_channel) == 3)
        {
            return GL_RGB;
        }

        if (static_cast<int>(_channel) == 4)
        {
            return GL_RGBA;
        }

        return GL_RED;
    }

    bool Image::loadFromFile(const std::filesystem::path& path, bool isFlipVertically)
    {
        if (!std::filesystem::exists(path)) [[unlikely]]
        {
            criticalLog("Provided path is invalid. Can't load image. Path: {}"_f
                        << path.lexically_normal().generic_string());
            return false;
        }

        const auto ext = path.extension().generic_string();
        if (!SupportedExtensions().contains(ext)) [[unlikely]]
        {
            criticalLog("Unsupported image extension '{}'. Path: {}"_f
                        << ext << path.lexically_normal().generic_string());
            return false;
        }

        clear();
        stbi_set_flip_vertically_on_load(isFlipVertically);
        int channel = 0;
        _data = stbi_load(path.string().c_str(), &_size.width, &_size.height, &channel, 0);
        if (isEmpty())
        {
            clear();
            criticalLog("stbi lib can't load the image. Path: {}"_f
                        << path.lexically_normal().generic_string());
            return false; // was missing
        }

        _channel = static_cast<Channel>(channel);
        _path = path;

        return true;
    }

    bool Image::loadFromMemory(const uint8_t* data, std::size_t size, bool isFlipVertically)
    {
        if (!data) [[unlikely]]
        {
            criticalLog("Provided data is nullptr. Can't load image.");
            return false;
        }

        clear();
        stbi_set_flip_vertically_on_load(isFlipVertically);
        int channel = 0;
        _data = stbi_load_from_memory(data, static_cast<int>(size), &_size.width, &_size.height,
                                      &channel, 0);
        if (isEmpty())
        {
            clear();
            criticalLog("stbi lib can't load the image from memory by unknown reasons");
        }

        _channel = static_cast<Channel>(channel);
        _path.clear();

        return true;
    }

    void Image::clear()
    {
        if (_data)
        {
            stbi_image_free(_data);
            _data = nullptr;
        }
        _size = {};
        _channel = Channel::None;
    }
} // namespace Core