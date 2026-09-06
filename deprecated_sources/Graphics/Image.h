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

#pragma once

#include "Core/Size.h"
#include "Misc/BaseLog.h"
#include "OpenGL.h"

#include <filesystem>
#include <string>
#include <unordered_set>

namespace Core
{
    class Image : public BaseLog
    {
    public:
        // The next values were taken from the stb_image.h documentation.
        ENUM_CLASS();
        enum class Channel
        {
            None = 0,
            Grey = 1,
            GreyA = 2,
            RGB = 3,
            RGBA = 4
        };

    public:
        Image() = default;
        explicit Image(const std::filesystem::path& path);
        ~Image() override;

        Image(Image&& obj) noexcept;
        Image& operator=(Image&& obj) noexcept;

        Image(const Image& obj) = delete;
        Image& operator=(const Image& obj) = delete;

        [[nodiscard]] ISize2 getSize() const noexcept { return _size; }
        [[nodiscard]] Channel getChannel() const noexcept { return _channel; }
        [[nodiscard]] int getChannelsCount() const noexcept { return static_cast<int>(_channel); }

        [[nodiscard]] GLenum getChannelAsOpenGLType() const noexcept;

        [[nodiscard]] const unsigned char* data() const noexcept { return _data; }

        bool loadFromFile(const std::filesystem::path& path, bool isFlipVertically = true,
                          bool forceRGBA = false);
        bool loadFromMemory(const uint8_t* data, std::size_t size, bool isFlipVertically = true);

        void clear();

        [[nodiscard]] bool isEmpty() const noexcept { return _data == nullptr; }
        [[nodiscard]] std::filesystem::path getPath() const noexcept { return _path.string(); }

        [[nodiscard]] spdlog::logger* getLogger() const override;
        [[nodiscard]] const char* getPrefix() const override { return "Image"; }

        friend void swap(Image& first, Image& second) noexcept
        {
            std::swap(first._path, second._path);
            std::swap(first._data, second._data);
            std::swap(first._size, second._size);
            std::swap(first._channel, second._channel);
        }

        [[nodiscard]] static std::unordered_set<std::string> SupportedExtensions();

    private:
        void init();

    private:
        std::filesystem::path _path;
        unsigned char* _data{};
        ISize2 _size;
        Channel _channel = Channel::None;
    };
} // namespace Core

#include "Image.generated.h" // added by the code generator. Better don't move it.
