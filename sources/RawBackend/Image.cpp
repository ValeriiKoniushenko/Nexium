// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "Image.h"

#include "PrivateModuleInfo.h"
#include "Stb/Image.h"

namespace RawBackend
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
        return RawBackend::getLogger();
    }

    std::unordered_set<std::string> Image::SupportedExtensions()
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

    bool Image::loadFromFile(const std::filesystem::path& path, bool isFlipVertically /* = true*/,
                             bool forceRGBA /* = false*/)
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
        _data = stbi_load(path.string().c_str(), &_size.width, &_size.height, &channel,
                          forceRGBA ? 4 : 0);
        if (isEmpty())
        {
            clear();
            criticalLog("stbi lib can't load the image. Path: {}"_f
                        << path.lexically_normal().generic_string());
            return false; // was missing
        }

        _channel = forceRGBA ? Channel::RGBA : static_cast<Channel>(channel);
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
} // namespace RawBackend