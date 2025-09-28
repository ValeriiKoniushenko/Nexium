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

#include "Core/Enum.h"
#include "Core/Size.h"
#include "Misc/BaseLog.h"
#include "ModuleInfo.h"
#include "OpenGL.h"
#include "Utils/CopyableAndMoveableBehaviour.h"
#include "glm/glm.hpp"

#include <filesystem>
#include <string>

namespace Core
{
    class Image : public Utils::NotCopyableButMoveable, public BaseLog
    {
    public:
        // The next values were taken from the stb_image.h documentation.
        // clang-format off
	    CreateEnum(Channel, int,
		    None = 0,
		    Grey = 1,
		    GreyA = 2,
		    RGB = 3,
		    RGBA = 4
	    );
        // clang-format on

    public:
        explicit Image(const std::filesystem::path& path = "");

        Image(Image&& obj) noexcept;

        Image& operator=(Image&& obj) noexcept;

        ~Image() override;

        [[nodiscard]] ISize2 getSize() const noexcept { return _size; }
        [[nodiscard]] Channel getChannel() const noexcept { return _channel; }
        [[nodiscard]] int getChannelsCount() const noexcept { return _channel.cast(); }

        [[nodiscard]] GLenum getChannelAsOpenGLType() const noexcept;

        [[nodiscard]] const unsigned char* data() const noexcept { return _data; }

        bool loadFromFile(const std::filesystem::path& path, bool isFlipVertically = true);
        bool loadFromMemory(const uint8_t* data, std::size_t size, bool isFlipVertically = true);

        void clear();

        [[nodiscard]] bool isEmpty() const noexcept { return _data == nullptr; }
        [[nodiscard]] StringAtom getName() const noexcept { return _name; }

        [[nodiscard]] spdlog::logger* getLogger() const override { return Graphics::getLogger(); }
        [[nodiscard]] const char* getPrefix() const override { return "Image"; }

    private:
        void init();

    private:
        unsigned char* _data{};
        ISize2 _size;
        Channel _channel = Channel::None;
        StringAtom _name;
    };
} // namespace Core