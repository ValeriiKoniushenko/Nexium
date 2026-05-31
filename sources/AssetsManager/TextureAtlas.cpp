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

#include "TextureAtlas.h"

#include "AssetsManager/ModuleInfo.h"
#include "Core/Rect.h"
#include "Core/String.h"
#include "Rectpack2D/finders_interface.h"

#include <filesystem>

namespace fs = std::filesystem;
using namespace rectpack2D;

using spaces_type = empty_spaces<true>;
using rect_type = output_rect_t<spaces_type>;

namespace Core
{
    void TextureAtlas::iterateOverFolderAndFetchImages(const std::filesystem::path& atlasFolder,
                                                       std::vector<Image>& images)
    {
        for (const auto& entry : fs::recursive_directory_iterator(atlasFolder))
        {
            if (!entry.is_regular_file())
            {
                continue;
            }

            const auto ext = entry.path().extension().string();

            if (ext == ".png" || ext == ".jpg" || ext == ".jpeg")
            {
                Image img;
                if (img.loadFromFile(entry.path()))
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
        const auto firstChannel = images.front().getChannelAsOpenGLType();
        for (const auto& img : images)
        {
            if (img.getChannelAsOpenGLType() != firstChannel)
            {
                criticalLog(
                    "All images in the atlas folder must have the same number of channels. Image '{}' has {} channels, while the first image has {} channels."_f
                    << img.getPath().generic_string()
                    << R<Image::Channel>::ToString(img.getChannel())
                    << R<Image::Channel>::ToString(images.front().getChannel()));
                return;
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
            rectangles, make_finder_input(max_side, discard_step, report_successful,
                                          report_unsuccessful, runtime_flipping_mode));

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
        return ::AssetsManager::getLogger();
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

} // namespace Core