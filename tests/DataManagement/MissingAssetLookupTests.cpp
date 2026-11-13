// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "NxFundamental/TextureAtlas/TextureAtlas.h"
#include "NxSubsystems/AssetsManager/AssetsManager.h"

#include "gtest/gtest.h"
#include <stdexcept>

// A scene file (or an animation asset) may reference a texture or an atlas that no longer
// exists: a renamed PNG, a removed sprite-sheet folder. The draw path runs every frame with no
// exception handler around it, so a throwing lookup there terminates the whole application
// instead of showing a broken sprite. The render path therefore needs non-throwing lookups;
// the throwing accessors stay for callers that treat a miss as a programming error.

TEST(MissingAssetLookupTests, AtlasRectLookupMissReportsNullInsteadOfThrowing)
{
    const NX::TextureAtlas atlas;

    EXPECT_THROW((void)atlas.getRect("missing.png"_atom), std::runtime_error);
    EXPECT_EQ(atlas.findRect("missing.png"_atom), nullptr);
}

TEST(MissingAssetLookupTests, AtlasLookupMissReportsNullInsteadOfThrowing)
{
    NX::AssetsManager assets;

    EXPECT_THROW((void)assets.getAtlas("no_such_atlas"_atom), std::runtime_error);
    EXPECT_EQ(assets.findAtlas("no_such_atlas"_atom), nullptr);
    EXPECT_EQ(assets.findAtlas("default"_atom), &assets.getAtlas("default"_atom));
}
