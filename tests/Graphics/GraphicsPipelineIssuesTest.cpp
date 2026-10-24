// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "NxFundamental/TextureAtlas/TextureAtlas.h"
#include "NxSubsystems/Graphics/Font.h"
#include "NxSubsystems/Graphics/GraphicsComponents.h"
#include "RawBackend/Image.h"
#include "RawBackend/Utils.h"
#include "Rectpack2D/finders_interface.h"

#include "gtest/gtest.h"
#include <array>
#include <type_traits>
#include <vector>

using namespace NX;

namespace
{
    // 1x1 PNG, colour type 4 (grey + alpha).
    constexpr std::array<std::uint8_t, 70> greyAlphaPng = {
        0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A, 0x00, 0x00, 0x00, 0x0D, 0x49, 0x48,
        0x44, 0x52, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x08, 0x04, 0x00, 0x00,
        0x00, 0xB5, 0x1C, 0x0C, 0x02, 0x00, 0x00, 0x00, 0x0B, 0x49, 0x44, 0x41, 0x54, 0x78,
        0x9C, 0x63, 0x68, 0xF8, 0x0F, 0x00, 0x02, 0x02, 0x01, 0x80, 0x6E, 0x56, 0x8B, 0x13,
        0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4E, 0x44, 0xAE, 0x42, 0x60, 0x82,
    };

    BaseGraphicsData MakeDataWithCullFaceDisabled()
    {
        BaseGraphicsData data;
        data.addDrawModifiers(BaseGraphicsData::ModifiedValue::CullFace,
                              BaseGraphicsData::Modifier::Disable);
        return data;
    }
} // namespace

TEST(GraphicsPipelineIssues, ImageLoadFromMemoryReportsFailureOnGarbage)
{
    constexpr std::array<std::uint8_t, 8> garbage = { 1, 2, 3, 4, 5, 6, 7, 8 };
    RawBackend::Image img;

    EXPECT_FALSE(img.loadFromMemory(garbage.data(), garbage.size()));
    EXPECT_TRUE(img.isEmpty());
}

TEST(GraphicsPipelineIssues, GreyAlphaImageMapsToTwoChannelGlFormat)
{
    RawBackend::Image img;
    ASSERT_TRUE(img.loadFromMemory(greyAlphaPng.data(), greyAlphaPng.size()));
    ASSERT_EQ(img.getChannel(), RawBackend::Image::Channel::GreyA);

    EXPECT_EQ(RawBackend::GetChannelAsOpenGLType(img), static_cast<GLenum>(GL_RG));
}

TEST(GraphicsPipelineIssues, BaseGraphicsDataMoveConstructorKeepsDrawModifiers)
{
    BaseGraphicsData src = MakeDataWithCullFaceDisabled();
    BaseGraphicsData moved(std::move(src));

    EXPECT_EQ(moved.getDrawModifier(BaseGraphicsData::ModifiedValue::CullFace),
              BaseGraphicsData::Modifier::Disable);
}

TEST(GraphicsPipelineIssues, BaseGraphicsDataMoveAssignmentKeepsDrawModifiers)
{
    BaseGraphicsData src = MakeDataWithCullFaceDisabled();
    BaseGraphicsData dst;
    dst = std::move(src);

    EXPECT_EQ(dst.getDrawModifier(BaseGraphicsData::ModifiedValue::CullFace),
              BaseGraphicsData::Modifier::Disable);
}

TEST(GraphicsPipelineIssues, GlyphVerticalOffsetIsSignedArithmetic)
{
    // Font.cpp computes `ch.size.height - ch.bearing.height`; glyphs whose bitmap_top exceeds
    // their bitmap rows ('-', '^', '"', '*', ...) need a negative result here.
    using G = Face::GlyphSize;
    using Diff = decltype(std::declval<G>().size.height - std::declval<G>().bearing.height);

    EXPECT_TRUE(std::is_signed_v<Diff>);

    const G dash{ .size = Core::ISize2{ 20, 3 }, .bearing = Core::ISize2{ 2, 14 }, .advance = 0 };
    EXPECT_LT(static_cast<float>(dash.size.height - dash.bearing.height), 0.f);
}

TEST(GraphicsPipelineIssues, AtlasPackerDoesNotRotateImages)
{
    // TextureAtlas packs with flipping enabled but uploads pixels and UVs ignoring `flipped`.
    using namespace rectpack2D;
    using spaces_type = empty_spaces<true>;
    using rect_type = output_rect_t<spaces_type>;

    std::vector<rect_type> rects;
    for (const auto [w, h] : std::array<std::pair<int, int>, 4>{ {
             { 256, 32 },
             { 32, 256 },
             { 256, 32 },
             { 32, 224 },
         } })
    {
        rect_type r;
        r.w = w;
        r.h = h;
        rects.push_back(r);
    }

    find_best_packing_dont_sort<spaces_type>(
        rects,
        make_finder_input(
            TextureAtlas::max_side, TextureAtlas::discard_step,
            [](auto&) { return callback_result::CONTINUE_PACKING; },
            [](auto&) { return callback_result::ABORT_PACKING; },
            TextureAtlas::allow_rotation ? flipping_option::ENABLED : flipping_option::DISABLED));

    for (const auto& r : rects)
    {
        EXPECT_FALSE(r.flipped) << "rect " << r.w << "x" << r.h << " was rotated by the packer";
    }
}
