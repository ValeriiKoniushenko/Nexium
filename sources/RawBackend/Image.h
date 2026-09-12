// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "Core/Size.h"
#include "Foundation/BaseLog.h"
#include "JustReflectMe/Adapter.h"

#include <filesystem>
#include <string>
#include <unordered_set>

namespace RawBackend
{
    class Image : public Foundation::BaseLog
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

        [[nodiscard]] Core::ISize2 getSize() const noexcept { return _size; }
        [[nodiscard]] Channel getChannel() const noexcept { return _channel; }
        [[nodiscard]] int getChannelsCount() const noexcept { return static_cast<int>(_channel); }

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
        Core::ISize2 _size;
        Channel _channel = Channel::None;
    };
} // namespace RawBackend

#include "Image.generated.h" // added by the code generator. Better don't move it.
