// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

// Documents what the direction helpers of Transformable do for a 2D object. They were written
// for a yaw/pitch (Y/X) camera: `_rotation.z` -- the only rotation a 2D sprite has -- is never
// consulted, and the "up" vector points to -Y at rest (moveUp() compensates by negating it).
// Gameplay code on a rotated sprite (moveRight()/moveUp(), "shoot along my facing") therefore
// moves along the world axes instead of the object's own. Expected to FAIL until the helpers
// honour the Z rotation and return a +Y up vector.

#include "NxFundamental/Transformable.h"

#include "gtest/gtest.h"

namespace
{
    void ExpectVecNear(const glm::vec3& actual, const glm::vec3& expected)
    {
        EXPECT_NEAR(actual.x, expected.x, 1.e-4f);
        EXPECT_NEAR(actual.y, expected.y, 1.e-4f);
        EXPECT_NEAR(actual.z, expected.z, 1.e-4f);
    }
} // namespace

TEST(TransformableDirectionGaps, UpIsPositiveYForAnUnrotatedObject)
{
    // The whole 2D pipeline is Y-up (glm::ortho with bottom < top; Rectangle's quad grows
    // towards +Y), so "up" for an unrotated object is +Y and "right" is +X.
    const NX::Transformable object;

    ExpectVecNear(object.getRightVector(), { 1.f, 0.f, 0.f });
    ExpectVecNear(object.getUpVector(), { 0.f, 1.f, 0.f });
}

TEST(TransformableDirectionGaps, RightAndUpFollowTheZRotation)
{
    // The model matrix rotates the sprite by +90 degrees around Z (counter-clockwise), which
    // maps local +X onto world +Y. The direction helpers must agree with the matrix.
    NX::Transformable object;
    object.setRotationZ(90.f);

    ExpectVecNear(object.getRightVector(), { 0.f, 1.f, 0.f });
    ExpectVecNear(object.getUpVector(), { -1.f, 0.f, 0.f });
}

TEST(TransformableDirectionGaps, MoveRightOnARotatedSpriteMovesAlongItsOwnAxis)
{
    NX::Transformable object;
    object.setRotationZ(90.f);

    object.moveRight(10.f);

    ExpectVecNear(object.getPosition(), { 0.f, 10.f, 0.f });
}
