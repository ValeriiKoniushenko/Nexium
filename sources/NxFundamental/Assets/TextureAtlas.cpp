// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "TextureAtlas.h"

#include "../PrivateModuleInfo.h"
#include "Core/Rect.h"
#include "Core/String.h"
#include "Rectpack2D/finders_interface.h"

#include <filesystem>

namespace fs = std::filesystem;

using namespace rectpack2D;
using namespace Core;
using namespace RawBackend;

using spaces_type = empty_spaces<true>;
using rect_type = output_rect_t<spaces_type>;

namespace
{
    GLenum GetChannelAsOpenGLType(const RawBackend::Image& data) noexcept
    {
        if (static_cast<int>(data.getChannel()) == 3)
        {
            return GL_RGB;
        }

        if (static_cast<int>(data.getChannel()) == 4)
        {
            return GL_RGBA;
        }

        return GL_RED;
    }
} // namespace

namespace NX
{

    std::vector<StringAtom> TextureAtlas::getRectsAsVector() const
    {
        std::vector<StringAtom> arr;
        arr.reserve(_rects.size());
        std::ranges::transform(_rects, std::back_inserter(arr),
                               [](const auto& pair) { return pair.first; });

        return arr;
    }

    void TextureAtlas::iterateOverFolderAndFetchImages(const std::filesystem::path& atlasFolder,
                                                       std::vector<Image>& images)
    {
        for (const auto& entry : fs::recursive_directory_iterator(atlasFolder))
        {
            if (!entry.is_regular_file())
            {
                continue;
            }

            const auto ext = entry.path().extension().generic_string();

            if (ext == ".png" || ext == ".jpg" || ext == ".jpeg")
            {
                Image img;
                if (img.loadFromFile(entry.path(), true, true))
                {
                    images.push_back(std::move(img));
                }
            }
        }
    }

    void TextureAtlas::generateTextureAtlas(const fs::path& atlasFolder)
    {
        std::vector<Image> images;
        std::vector<rect_type> rectangles;

        iterateOverFolderAndFetchImages(atlasFolder, images);

        if (images.empty())
        {
            warnLog("No images found in the atlas folder '{}'"_f << atlasFolder.generic_string());
            return;
        }

        // validation for data consistency
        const auto firstChannel = GetChannelAsOpenGLType(images.front());
        for (const auto& img : images)
        {
            if (GetChannelAsOpenGLType(img) != firstChannel)
            {
                criticalLog(
                    "All images in the atlas folder must have the same number of channels. Image '{}' has {} channels, while the first(anchored) image has {} channels."_f
                    << img.getPath().generic_string()
                    << R<Image::Channel>::ToString(img.getChannel())
                    << R<Image::Channel>::ToString(images.front().getChannel()));
                // return;
            }
        }

        for (const auto& img : images)
        {
            rect_type rt;
            rt.w = img.getSize().width;
            rt.h = img.getSize().height;
            rectangles.push_back(rt);
        }

        int discarded = 0;
        auto report_successful = [](auto) { return callback_result::CONTINUE_PACKING; };
        auto report_unsuccessful = [&discarded](auto)
        {
            ++discarded;
            return callback_result::ABORT_PACKING;
        };

        const auto result_size = find_best_packing_dont_sort<spaces_type>(
            rectangles,
            make_finder_input(max_side, discard_step, report_successful, report_unsuccessful,
                              rectpack2D::flipping_option::ENABLED));

        if (discarded)
        {
            errorLog(
                "Couldn't fit {} images into the atlas. Consider increasing max_side or decreasing discard_step"_f
                << discarded);
        }

        if (result_size.w == 0 || result_size.h == 0)
        {
            warnLog("Atlas size is W x H: 0 x 0");
            return;
        }

        _texture.generate();
        _texture.bind();

        _texture.putImage(0, GL_RGBA, result_size.w, result_size.h, 0, firstChannel,
                          GL_UNSIGNED_BYTE, nullptr);

        for (std::size_t i = 0; i < images.size(); ++i)
        {
            const auto& rect = rectangles[i];
            const auto name = fs::relative(images[i].getPath(), atlasFolder).generic_string();
            auto& r = _rects[StringAtom::Intern(name)];

            const int flipped_y = rect.y;

            r.setLeftTop(GlobalPosition2F(
                static_cast<float>(rect.x) / static_cast<float>(result_size.w),
                static_cast<float>(flipped_y) / static_cast<float>(result_size.h)));

            r.setRightBottom(GlobalPosition2F(
                static_cast<float>(rect.x + rect.w) / static_cast<float>(result_size.w),
                static_cast<float>(flipped_y + rect.h) / static_cast<float>(result_size.h)));

            _texture.putSubImage(0, rect.x, rect.y, rect.w, rect.h, firstChannel, GL_UNSIGNED_BYTE,
                                 images[i].data());
        }

        _texture.generateMipmap();
        _texture.unbind();
    }

    void TextureAtlas::bind() const
    {
        Assert(_texture.isValid());
        _texture.bind();
    }

    void TextureAtlas::unbind() const
    {
        _texture.unbind();
    }

    void TextureAtlas::activateTextureUnit(GLuint unit) const
    {
        _texture.activateTextureUnit(unit);
    }

    spdlog::logger* TextureAtlas::getLogger() const
    {
        return NxFundamental::getLogger();
    }

    const FRect& TextureAtlas::getRect(const StringAtom& name) const
    {
        auto it = _rects.find(name);
        if (it == _rects.end()) [[unlikely]]
        {
            throw std::runtime_error("TextureAtlas: No rect with name '" + name.toStdString()
                                     + "' found");
        }
        return it->second;
    }

} // namespace NX