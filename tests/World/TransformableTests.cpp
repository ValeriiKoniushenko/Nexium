// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "NxFundamental/Transformable.h"
#include "NxWorld/Scene/SceneObjects/Rectangle/Rectangle.h"
#include "glm/gtc/matrix_transform.hpp"

#include "gtest/gtest.h"

namespace
{
    void ExpectMatrixNear(const glm::mat4& actual, const glm::mat4& expected)
    {
        for (int column = 0; column < 4; ++column)
        {
            for (int row = 0; row < 4; ++row)
            {
                EXPECT_NEAR(actual[column][row], expected[column][row], 1.e-4f);
            }
        }
    }
} // namespace

TEST(TransformableTests, GetPositionReturnsWhatSetPositionStoredRegardlessOfOrigin)
{
    NX::Transformable object;
    object.setOrigin({ 50.f, 50.f, 0.f });
    object.setPosition(NX::GPos3{ 100.f, 200.f, 0.f });

    // A setter/getter pair must round-trip; otherwise every editor edit of the position
    // field (which writes back what it displayed) drifts the object by its origin.
    EXPECT_EQ(object.getPosition(), NX::GPos3(100.f, 200.f, 0.f));
}

TEST(TransformableTests, PositionIsTheWorldPositionOfThePivot)
{
    // The model matrix rotates and scales around the origin, so the pivot (local `origin`)
    // lands at `position` in parent space. The gizmo relies on this when it writes positions.
    NX::Transformable object;
    object.setOrigin({ 50.f, 50.f, 0.f });
    object.setScale({ 2.f, 3.f, 1.f });
    object.setRotationZ(35.f);
    object.setPosition(NX::GPos3{ 100.f, 200.f, 0.f });
    object.recalculateMatrices();

    const glm::vec3 pivotInParent = object.getModelMatrix() * glm::vec4(object.getOrigin(), 1.f);
    EXPECT_NEAR(pivotInParent.x, object.getPosition().x, 1.e-4f);
    EXPECT_NEAR(pivotInParent.y, object.getPosition().y, 1.e-4f);
    EXPECT_NEAR(pivotInParent.z, object.getPosition().z, 1.e-4f);
}

TEST(TransformableTests, SceneStateRoundTripKeepsTheTransform)
{
    // Scene::deserialize restores objects with setTransformations(state.trans); saving and
    // loading a scene must not move objects that use a non-zero origin.
    NX::SceneObj::Rectangle saved;
    saved.setOrigin({ 50.f, 50.f, 0.f });
    saved.setScale({ 2.f, 2.f, 1.f });
    saved.setRotationZ(10.f);
    saved.setPosition(NX::GPos3{ 100.f, 200.f, 0.f });
    saved.recalculateMatrices();

    NX::SceneObj::Rectangle loaded;
    loaded.setTransformations(saved.getSceneState().trans);
    loaded.recalculateMatrices();

    EXPECT_EQ(loaded.getPosition(), saved.getPosition());
    EXPECT_EQ(loaded.getOrigin(), saved.getOrigin());
    ExpectMatrixNear(loaded.getModelMatrix(), saved.getModelMatrix());

    // And a second round trip is still a no-op.
    NX::SceneObj::Rectangle reloaded;
    reloaded.setTransformations(loaded.getSceneState().trans);
    reloaded.recalculateMatrices();
    ExpectMatrixNear(reloaded.getModelMatrix(), saved.getModelMatrix());
}
