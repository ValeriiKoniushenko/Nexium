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

#include "Image.h"

#include "ModuleInfo.h"
#define STB_IMAGE_IMPLEMENTATION
#include "Stb/Image.h"


#include "Image.generated.cpp.inl" // this line added by the code generator.

namespace Core
{
    Image::Image(const std::filesystem::path& path)
    {
        init();
        if (!path.empty())
        {
            loadFromFile(path);
        }
    }

    Image::~Image()
    {
        clear();
    }

    Image::Image(Image&& obj) noexcept
    {
        init();
        *this = std::move(obj);
    }

    Image& Image::operator=(Image&& obj) noexcept
    {
        clear();
        this->_data = obj._data;
        this->_size = obj._size;
        this->_channel = obj._channel;
        obj.init();
        return *this;
    }

    spdlog::logger* Image::getLogger() const
    {
        return Graphics::getLogger();
    }

    void Image::init()
    {
        _data = nullptr;
        _size = {};
        _channel = Channel::None;
    }

    GLenum Image::getChannelAsOpenGLType() const noexcept
    {
        return static_cast<int>(_channel) == 3 ? GL_RGB : static_cast<int>(_channel) == 4 ? GL_RGBA : GL_RED;
    }

    bool Image::loadFromFile(const std::filesystem::path& path, bool isFlipVertically)
    {
        if (!std::filesystem::exists(path))
        {
            criticalLog("Provided path is invalid. Can't load image. Path: {}"_f
                        << path.lexically_normal().generic_string());
            return false;
        }

        clear();
        stbi_set_flip_vertically_on_load(isFlipVertically);
        int channel = 0;
        _data = stbi_load(path.string().c_str(), &_size.width, &_size.height, &channel, 0);
        if (isEmpty())
        {
            clear();
            criticalLog("stbi lib can't load the image by unknown reasons. Image path: {}"_f
                        << path.lexically_normal().generic_string());
        }

        _channel = static_cast<Channel>(channel);
        _name = path.lexically_normal().generic_string();

        return true;
    }

    bool Image::loadFromMemory(const uint8_t* data, std::size_t size, bool isFlipVertically)
    {
        if (!data)
        {
            criticalLog("Provided data is nullptr. Can't load image.");
            return false;
        }

        clear();
        stbi_set_flip_vertically_on_load(isFlipVertically);
        int channel = 0;
        _data = stbi_load_from_memory(data, size, &_size.width, &_size.height, &channel, 0);
        if (isEmpty())
        {
            clear();
            criticalLog("stbi lib can't load the image from memory by unknown reasons");
        }

        _channel = static_cast<Channel>(channel);
        _name = "None"_atom;

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