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

#include "Core/Rect.h"
#include "Core/String.h"
#include "Graphics/Image.h"
#include "Graphics/Texture.h"
#include "Misc/BaseLog.h"
#include "Rectpack2D/empty_spaces.h"

#include <filesystem>
#include <unordered_map>

namespace Core
{

    class TextureAtlas final : public BaseLog
    {
    public:
        constexpr static const int max_side = 1000;
        constexpr static const int discard_step = -4;
        constexpr static const auto runtime_flipping_mode = rectpack2D::flipping_option::ENABLED;

    public:
        TextureAtlas() = default;
        ~TextureAtlas() override = default;
        TextureAtlas(const TextureAtlas&) = delete;
        TextureAtlas(TextureAtlas&&) = delete;
        TextureAtlas& operator=(const TextureAtlas&) = delete;
        TextureAtlas& operator=(TextureAtlas&&) = delete;

        void generateTextureAtlas(const std::filesystem::path& atlasFolder);

        void bind() const;
        void unbind() const;
        void activateTextureUnit(GLuint unit) const;

        [[nodiscard]] spdlog::logger* getLogger() const override;

        [[nodiscard]] const Texture& getTexture() const noexcept { return _texture; }

        [[nodiscard]] const FRect& getRect(const StringAtom& name) const;

        [[nodiscard]] const std::unordered_map<StringAtom, FRect>& getRects() const noexcept
        {
            return _rects;
        }

    private:
        void iterateOverFolderAndFetchImages(const std::filesystem::path& atlasFolder,
                                             std::vector<Image>& images);

    private:
        Texture _texture;
        std::unordered_map<StringAtom, FRect> _rects;
    };
} // namespace Core