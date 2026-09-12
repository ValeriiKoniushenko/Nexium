#include "ImageAlphaMask.h"

#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace Core
{
    std::optional<FRect> ImageAlphaMask::UnionBounds(std::span<const std::optional<FRect>> bounds)
    {
        std::optional<FRect> result;
        for (const auto& rect : bounds)
        {
            if (!rect)
            {
                continue;
            }
            if (!result)
            {
                result = rect;
                continue;
            }
            const auto low
                = glm::min(glm::vec2(result->getLeftBottom()), glm::vec2(rect->getLeftBottom()));
            const auto high
                = glm::max(glm::vec2(result->getRightTop()), glm::vec2(rect->getRightTop()));
            result.emplace(low.x, high.y, high.x, low.y);
        }
        return result;
    }

    ImageAlphaMask::ImageAlphaMask(std::span<const unsigned char> pixels, ISize2 size, int channels)
        : _size(size)
    {
        if (size.width <= 0 || size.height <= 0 || channels < 1 || channels > 4)
        {
            throw std::invalid_argument("Invalid image dimensions or channels for alpha mask");
        }
        const auto count = static_cast<std::size_t>(size.width) * size.height;
        if (count > pixels.size() / static_cast<std::size_t>(channels))
        {
            throw std::invalid_argument("Insufficient image data for alpha mask");
        }
        _alpha.resize(count, 255);
        if (channels == 2 || channels == 4)
        {
            for (std::size_t i = 0; i < count; ++i)
            {
                _alpha[i] = pixels[i * channels + channels - 1];
            }
        }
    }

    std::optional<FRect> ImageAlphaMask::getAlphaBounds(glm::vec2 offset, glm::vec2 size) const
    {
        const auto end = offset + size;
        if (!std::isfinite(offset.x) || !std::isfinite(offset.y) || !std::isfinite(end.x)
            || !std::isfinite(end.y) || size.x == 0.f || size.y == 0.f)
        {
            return std::nullopt;
        }
        const auto low = glm::min(offset, end);
        const auto high = glm::max(offset, end);
        if (low.x < 0.f || low.y < 0.f || high.x > 1.f || high.y > 1.f)
        {
            return std::nullopt;
        }
        // Avoid including a neighbouring frame due to float UV rounding.
        const auto snap = [](float value)
        {
            const auto rounded = std::round(value);
            return std::abs(value - rounded) < 0.0001f ? rounded : value;
        };
        const auto imageSize = glm::vec2(_size.toGlm());
        const std::array<int, 4> region{ static_cast<int>(std::floor(snap(low.x * imageSize.x))),
                                         static_cast<int>(std::floor(snap(low.y * imageSize.y))),
                                         static_cast<int>(std::ceil(snap(high.x * imageSize.x))),
                                         static_cast<int>(std::ceil(snap(high.y * imageSize.y))) };
        auto it = _bounds.find(region);
        if (it == _bounds.end())
        {
            int left = region[2];
            int bottom = region[3];
            int right = region[0];
            int top = region[1];
            for (int y = region[1]; y < region[3]; ++y)
            {
                for (int x = region[0]; x < region[2]; ++x)
                {
                    if (_alpha[static_cast<std::size_t>(y) * _size.width + x] != 0)
                    {
                        left = std::min(left, x);
                        bottom = std::min(bottom, y);
                        right = std::max(right, x + 1);
                        top = std::max(top, y + 1);
                    }
                }
            }
            std::optional<IRect> bounds;
            if (left < right && bottom < top)
            {
                bounds.emplace(left, top, right, bottom);
            }
            it = _bounds.emplace(region, bounds).first;
        }
        if (!it->second)
        {
            return std::nullopt;
        }
        const auto a = (glm::vec2(it->second->getLeftBottom()) / imageSize - offset) / size;
        const auto b = (glm::vec2(it->second->getRightTop()) / imageSize - offset) / size;
        const auto rectLow = glm::clamp(glm::min(a, b), glm::vec2(0.f), glm::vec2(1.f));
        const auto rectHigh = glm::clamp(glm::max(a, b), glm::vec2(0.f), glm::vec2(1.f));
        return FRect(rectLow.x, rectHigh.y, rectHigh.x, rectLow.y);
    }
} // namespace Core
