#pragma once

#include "Core/Rect.h"

#include <array>
#include <cstdint>
#include <map>
#include <optional>
#include <span>
#include <vector>

namespace Core
{
    // Keeps one alpha byte per pixel. Rows follow the uploaded image's orientation.
    class ImageAlphaMask
    {
    public:
        [[nodiscard]] static std::optional<FRect> UnionBounds(
            std::span<const std::optional<FRect>> bounds);

        ImageAlphaMask(std::span<const unsigned char> pixels, ISize2 size, int channels);

        // Bounds of alpha > 0, relative to the selected UV region in [0, 1].
        // FRect uses Y-up coordinates: top >= bottom. Empty/invalid regions return nullopt.
        // Supports mirrored UVs within the image. Pixel bounds are cached per region.
        [[nodiscard]] std::optional<FRect> getAlphaBounds(glm::vec2 offset = { 0.f, 0.f },
                                                          glm::vec2 size = { 1.f, 1.f }) const;

    private:
        ISize2 _size;
        std::vector<uint8_t> _alpha;
        mutable std::map<std::array<int, 4>, std::optional<IRect>> _bounds;
    };
} // namespace Core
