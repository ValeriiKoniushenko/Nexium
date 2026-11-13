// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "NxSubsystems/Graphics/Font.h"

#include "gtest/gtest.h"

using NX::Face;

// FreeType semantics: `bearing.height` (bitmap_top) is the distance from the baseline up to
// the top row of the bitmap; the bitmap is `size.height` rows tall. The text pipeline is Y-up
// (same camera as the rest of the 2D scene), so the quad's bottom edge sits at
// bearing.height - size.height relative to the baseline.

TEST(FontLayoutTests, DescenderGlyphHangsBelowTheBaseline)
{
    // 'g': 35 rows tall, 25 of them above the baseline.
    const Face::GlyphSize glyph{ .size = Core::ISize2{ 20, 35 },
                                 .bearing = Core::ISize2{ 2, 25 },
                                 .advance = 0 };

    const auto origin = Face::GetGlyphQuadOrigin(glyph, 0.f);
    EXPECT_FLOAT_EQ(origin.y, -10.f);
    EXPECT_FLOAT_EQ(origin.y + static_cast<float>(glyph.size.height), 25.f);
}

TEST(FontLayoutTests, DashGlyphFloatsAboveTheBaseline)
{
    // '-': 3 rows tall, whose top is 14 px above the baseline.
    const Face::GlyphSize glyph{ .size = Core::ISize2{ 20, 3 },
                                 .bearing = Core::ISize2{ 2, 14 },
                                 .advance = 0 };

    const auto origin = Face::GetGlyphQuadOrigin(glyph, 0.f);
    EXPECT_FLOAT_EQ(origin.y, 11.f);
    EXPECT_FLOAT_EQ(origin.y + static_cast<float>(glyph.size.height), 14.f);
}

TEST(FontLayoutTests, BaselineGlyphStartsAtTheBaselineAndHonoursLeftBearing)
{
    // 'H': the whole bitmap is above the baseline.
    const Face::GlyphSize glyph{ .size = Core::ISize2{ 24, 30 },
                                 .bearing = Core::ISize2{ 3, 30 },
                                 .advance = 0 };

    const auto origin = Face::GetGlyphQuadOrigin(glyph, 100.f);
    EXPECT_FLOAT_EQ(origin.y, 0.f);
    EXPECT_FLOAT_EQ(origin.x, 103.f);
}
