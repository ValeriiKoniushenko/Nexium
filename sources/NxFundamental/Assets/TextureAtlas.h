// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "Core/Rect.h"
#include "Core/String.h"
#include "Foundation/BaseLog.h"
#include "RawBackend/Graphics/Texture.h"
#include "RawBackend/Image.h"

#include <filesystem>
#include <unordered_map>

namespace NX
{

    class TextureAtlas final : public Foundation::BaseLog
    {
    public:
        constexpr static const int max_side = 4096;
        constexpr static const int discard_step = -4;

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

        [[nodiscard]] const RawBackend::Texture& getTexture() const noexcept { return _texture; }

        [[nodiscard]] const Core::FRect& getRect(const Core::StringAtom& name) const;

        [[nodiscard]] const std::unordered_map<Core::StringAtom, Core::FRect>& getRects()
            const noexcept
        {
            return _rects;
        }

        [[nodiscard]] std::vector<Core::StringAtom> getRectsAsVector() const;

        [[nodiscard]] std::size_t getRectsCount() const noexcept { return _rects.size(); }

    private:
        void iterateOverFolderAndFetchImages(const std::filesystem::path& atlasFolder,
                                             std::vector<RawBackend::Image>& images);

    private:
        RawBackend::Texture _texture;
        std::unordered_map<Core::StringAtom, Core::FRect> _rects;
    };
} // namespace NX
