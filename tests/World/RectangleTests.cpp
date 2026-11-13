// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "NxWorld/Scene/SceneObjects/Rectangle/Rectangle.h"
#include "NxWorld/Scene/SceneObjects/SceneObject.h"

#include "gtest/gtest.h"

namespace
{
    void ExpectCornerNear(const glm::vec3& actual, glm::vec2 expected)
    {
        EXPECT_NEAR(actual.x, expected.x, 1.e-3f);
        EXPECT_NEAR(actual.y, expected.y, 1.e-3f);
        EXPECT_NEAR(actual.z, 0.f, 1.e-3f);
    }
} // namespace

// The outline (and anything else that needs the rectangle's footprint) must follow the
// geometry the shader draws: the unit quad [0, 100]^2 transformed by the model matrix.

TEST(RectangleTests, WorldCornersFollowPositionAndScale)
{
    NX::SceneObj::Rectangle rectangle;
    rectangle.setPosition(NX::GPos3{ 100.f, 200.f, 0.f });
    rectangle.setScale({ 2.f, 0.5f, 1.f });

    const auto corners = rectangle.getWorldCorners();
    ExpectCornerNear(corners[0], { 100.f, 200.f });
    ExpectCornerNear(corners[1], { 300.f, 200.f });
    ExpectCornerNear(corners[2], { 300.f, 250.f });
    ExpectCornerNear(corners[3], { 100.f, 250.f });
}

TEST(RectangleTests, WorldCornersRespectTheOriginAsPivot)
{
    // With the origin in the middle of the quad, scaling grows the sprite around its centre
    // and the pivot stays at `position`: the drawn quad spans position +/- 100.
    NX::SceneObj::Rectangle rectangle;
    rectangle.setOrigin({ 50.f, 50.f, 0.f });
    rectangle.setScale({ 2.f, 2.f, 1.f });
    rectangle.setPosition(NX::GPos3{ 100.f, 100.f, 0.f });

    const auto corners = rectangle.getWorldCorners();
    ExpectCornerNear(corners[0], { 0.f, 0.f });
    ExpectCornerNear(corners[1], { 200.f, 0.f });
    ExpectCornerNear(corners[2], { 200.f, 200.f });
    ExpectCornerNear(corners[3], { 0.f, 200.f });
}

TEST(RectangleTests, WorldCornersRotateAroundTheOrigin)
{
    NX::SceneObj::Rectangle rectangle;
    rectangle.setOrigin({ 50.f, 50.f, 0.f });
    rectangle.setRotationZ(90.f);
    rectangle.setPosition(NX::GPos3{ 100.f, 100.f, 0.f });

    // Rotating a centred square by 90 degrees keeps its footprint; the corners cycle.
    const auto corners = rectangle.getWorldCorners();
    ExpectCornerNear(corners[0], { 150.f, 50.f });
    ExpectCornerNear(corners[1], { 150.f, 150.f });
    ExpectCornerNear(corners[2], { 50.f, 150.f });
    ExpectCornerNear(corners[3], { 50.f, 50.f });
}

TEST(RectangleTests, WorldCornersIncludeParentTransform)
{
    NX::SceneObject parent;
    parent.setPosition(NX::GPos3{ 1000.f, 0.f, 0.f });
    parent.setScale({ 2.f, 2.f, 1.f });
    auto* child = parent.addChildComponent<NX::SceneObj::Rectangle>();
    child->setPosition(NX::GPos3{ 10.f, 10.f, 0.f });
    parent.recalculateMatrices();

    const auto corners = child->getWorldCorners();
    ExpectCornerNear(corners[0], { 1020.f, 20.f });
    ExpectCornerNear(corners[2], { 1220.f, 220.f });
}
