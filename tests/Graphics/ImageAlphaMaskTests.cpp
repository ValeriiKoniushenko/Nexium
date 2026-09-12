#include "Graphics/Image.h"
#include "Graphics/ImageAlphaMask.h"

#include "gtest/gtest.h"
#include <limits>
#include <stdexcept>

using namespace Core;

namespace
{
    void ExpectRect(const std::optional<FRect>& rect, float left, float top, float right,
                    float bottom)
    {
        if (!rect)
        {
            ADD_FAILURE() << "Expected visible bounds";
            return;
        }
        EXPECT_TRUE(rect->isValid());
        EXPECT_NEAR(rect->getLeftTop().x, left, 0.00001f);
        EXPECT_NEAR(rect->getLeftTop().y, top, 0.00001f);
        EXPECT_NEAR(rect->getRightBottom().x, right, 0.00001f);
        EXPECT_NEAR(rect->getRightBottom().y, bottom, 0.00001f);
    }
} // namespace

TEST(ImageAlphaMask, RemovesTransparentMarginsAndIncludesPartiallyOpaquePixels)
{
    std::array<unsigned char, 4uz * 4 * 4> pixels{};
    pixels[(1 * 4 + 1) * 4 + 3] = 1;
    pixels[(2 * 4 + 2) * 4 + 3] = 255;
    const ImageAlphaMask mask(pixels, ISize2{ 4, 4 }, 4);
    ExpectRect(mask.getAlphaBounds(), 0.25f, 0.75f, 0.75f, 0.25f);
    ExpectRect(mask.getAlphaBounds(), 0.25f, 0.75f, 0.75f, 0.25f);
}

TEST(ImageAlphaMask, UsesCurrentFrameAndPreservesBottomUpOrientation)
{
    std::array<unsigned char, 6uz * 3 * 4> pixels{};
    pixels[(0 * 6 + 1) * 4 + 3] = 255;
    pixels[(2 * 6 + 5) * 4 + 3] = 255;
    const ImageAlphaMask mask(pixels, ISize2{ 6, 3 }, 4);
    ExpectRect(mask.getAlphaBounds({ 0.f, 0.f }, { 0.5f, 1.f }), 1.f / 3.f, 1.f / 3.f, 2.f / 3.f,
               0.f);
    ExpectRect(mask.getAlphaBounds({ 0.5f, 0.f }, { 0.5f, 1.f }), 2.f / 3.f, 1.f, 1.f, 2.f / 3.f);
    EXPECT_FALSE(mask.getAlphaBounds({ 0.f, 1.f / 3.f }, { 1.f, 1.f / 3.f }));
}

TEST(ImageAlphaMask, SupportsGreyAlphaAndMirroredFrames)
{
    const std::array<unsigned char, 8> pixels{ 0, 255, 0, 0, 0, 0, 0, 0 };
    const ImageAlphaMask mask(pixels, ISize2{ 2, 2 }, 2);
    ExpectRect(mask.getAlphaBounds(), 0.f, 0.5f, 0.5f, 0.f);
    ExpectRect(mask.getAlphaBounds({ 1.f, 1.f }, { -1.f, -1.f }), 0.5f, 1.f, 1.f, 0.5f);
}

TEST(ImageAlphaMask, TreatsFormatsWithoutAlphaAsOpaque)
{
    const std::array<unsigned char, 12> pixels{};
    ExpectRect(ImageAlphaMask(pixels, ISize2{ 2, 2 }, 3).getAlphaBounds(), 0.f, 1.f, 1.f, 0.f);
    ExpectRect(ImageAlphaMask(pixels, ISize2{ 2, 2 }, 1).getAlphaBounds(), 0.f, 1.f, 1.f, 0.f);
}

TEST(ImageAlphaMask, ClipsFractionalPixelBoundsToTheSelectedRegion)
{
    const std::array<unsigned char, 4> pixels{ 0, 0, 0, 255 };
    const ImageAlphaMask mask(pixels, ISize2{ 1, 1 }, 4);
    ExpectRect(mask.getAlphaBounds({ 0.2f, 0.3f }, { 0.4f, 0.5f }), 0.f, 1.f, 1.f, 0.f);
}

TEST(ImageAlphaMask, RejectsTransparentAndInvalidRegions)
{
    const std::array<unsigned char, 16> pixels{};
    const ImageAlphaMask mask(pixels, ISize2{ 2, 2 }, 4);
    EXPECT_FALSE(mask.getAlphaBounds());
    EXPECT_FALSE(mask.getAlphaBounds());
    EXPECT_FALSE(mask.getAlphaBounds({ 0.f, 0.f }, { 0.f, 1.f }));
    EXPECT_FALSE(mask.getAlphaBounds({ -0.5f, 0.f }, { 1.f, 1.f }));
    EXPECT_FALSE(mask.getAlphaBounds({ 0.5f, 0.f }, { 1.f, 1.f }));
    EXPECT_FALSE(mask.getAlphaBounds({ std::numeric_limits<float>::quiet_NaN(), 0.f }));
    EXPECT_FALSE(mask.getAlphaBounds({}, { std::numeric_limits<float>::infinity(), 1.f }));
}

TEST(ImageAlphaMask, RejectsInvalidPixelBuffers)
{
    const std::array<unsigned char, 1> pixels{};
    EXPECT_THROW((ImageAlphaMask(pixels, ISize2{ 1, 1 }, 4)), std::invalid_argument);
    EXPECT_THROW((ImageAlphaMask(pixels, ISize2{ 0, 1 }, 1)), std::invalid_argument);
    EXPECT_THROW((ImageAlphaMask(pixels, ISize2{ 1, 1 }, 5)), std::invalid_argument);
}

TEST(ImageAlphaMask, MatchesImageLoaderVerticalFlip)
{
    // Uncompressed 2x2 RGBA TGA, top-left origin, one visible pixel at top-left.
    std::array<unsigned char, 18 + 2 * 2 * 4> encoded{};
    encoded[2] = 2;
    encoded[12] = 2;
    encoded[14] = 2;
    encoded[16] = 32;
    encoded[17] = 0x28;
    encoded[21] = 255;
    Image image;
    ASSERT_TRUE(image.loadFromMemory(encoded.data(), encoded.size(), true));
    ASSERT_EQ(image.getChannelsCount(), 4);
    const ImageAlphaMask mask(std::span<const unsigned char>(image.data(), 16), image.getSize(),
                              image.getChannelsCount());
    ExpectRect(mask.getAlphaBounds(), 0.f, 1.f, 0.5f, 0.5f);
}

TEST(ImageAlphaMask, DoesNotIncludeAdjacentFrameDueToUVRounding)
{
    std::array<unsigned char, 100uz * 4> pixels{};
    pixels[60 * 4 + 3] = 255;
    const ImageAlphaMask mask(pixels, ISize2{ 100, 1 }, 4);
    EXPECT_FALSE(mask.getAlphaBounds({ 0.5f, 0.f }, { 0.1f, 1.f }));
    ExpectRect(mask.getAlphaBounds({ 0.6f, 0.f }, { 0.1f, 1.f }), 0.f, 1.f, 0.1f, 0.f);
}

TEST(ImageAlphaMask, AnimationBoundsCombineOffsetsRatherThanChoosingLargestFrame)
{
    std::array<unsigned char, 8uz * 4 * 4> pixels{};
    pixels[(0 * 8 + 1) * 4 + 3] = 255;
    pixels[(2 * 8 + 3) * 4 + 3] = 255;
    pixels[(3 * 8 + 4) * 4 + 3] = 255;
    const ImageAlphaMask mask(pixels, ISize2{ 8, 4 }, 4);
    const std::array frames{ mask.getAlphaBounds({ 0.f, 0.f }, { 0.5f, 1.f }),
                             mask.getAlphaBounds({ 0.5f, 0.f }, { 0.5f, 1.f }) };
    ExpectRect(ImageAlphaMask::UnionBounds(frames), 0.f, 1.f, 1.f, 0.f);
    const std::array reversed{ frames[1], frames[0] };
    ExpectRect(ImageAlphaMask::UnionBounds(reversed), 0.f, 1.f, 1.f, 0.f);
}

TEST(ImageAlphaMask, AnimationBoundsIgnoreTransparentFrames)
{
    const std::array<std::optional<FRect>, 3> frames{ std::nullopt, FRect(0.2f, 0.8f, 0.6f, 0.1f),
                                                      std::nullopt };
    ExpectRect(ImageAlphaMask::UnionBounds(frames), 0.2f, 0.8f, 0.6f, 0.1f);
    const std::array<std::optional<FRect>, 2> transparent{};
    EXPECT_FALSE(ImageAlphaMask::UnionBounds(transparent));
    EXPECT_FALSE(ImageAlphaMask::UnionBounds({}));
}

TEST(ImageAlphaMask, AnimationBoundsNormalizeFramesFromDifferentTextures)
{
    std::array<unsigned char, 16> small{};
    small[3] = 255;
    std::array<unsigned char, 64> large{};
    large[(3 * 4 + 3) * 4 + 3] = 255;
    const ImageAlphaMask first(small, ISize2{ 2, 2 }, 4);
    const ImageAlphaMask second(large, ISize2{ 4, 4 }, 4);
    const std::array frames{ first.getAlphaBounds(), second.getAlphaBounds() };
    ExpectRect(ImageAlphaMask::UnionBounds(frames), 0.f, 1.f, 1.f, 0.f);
}
