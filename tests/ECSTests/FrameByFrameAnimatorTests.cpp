// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "NxWorld/Animations/FrameByFrame/FrameByFrameAnimator.h"
#include "NxWorld/Scene/SceneObjects/Rectangle/RectangleAnimated.h"

#include "gtest/gtest.h"

TEST(FrameByFrameAnimatorTests, TickBeforeSelectingAnimationPreservesRectangle)
{
    NX::SceneObj::RectangleAnimated rectangle;
    rectangle.setAtlas("initial atlas"_atom);
    rectangle.setTexture("initial texture"_atom);
    auto* animator = rectangle.addChildComponent<NX::Animation::FrameByFrameAnimator>();

    EXPECT_EQ(animator->getActiveAnimation(), nullptr);
    animator->tick(0.016f);

    EXPECT_EQ(rectangle.getAtlasName(), "initial atlas"_atom);
    EXPECT_EQ(rectangle.getTextureName(), "initial texture"_atom);
    EXPECT_EQ(animator->getActiveAnimation(), nullptr);
}
