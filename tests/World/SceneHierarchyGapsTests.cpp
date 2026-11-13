// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

// Tests in this file document behaviour that the scene hierarchy promises but does not deliver
// yet. They are expected to FAIL until the corresponding logic exists; each comment explains
// why the asserted behaviour is the one a user of the engine relies on.

#include "NxWorld/Scene/SceneObjects/Rectangle/Rectangle.h"
#include "NxWorld/Scene/SceneObjects/SceneObject.h"

#include "gtest/gtest.h"

TEST(SceneHierarchyGaps, MovingOnlyAChildUpdatesItsModelMatrix)
{
    // Dirtiness propagates downwards only (SceneObject::onDirtyMatrix marks descendants), and
    // matrices are recalculated top-down from whoever calls tryToRecalculateMatrices() on the
    // root (Rectangle::onDraw, the gizmo, ...). When only a child is edited (properties window,
    // gameplay code calling setPosition on a child), the root is clean, nothing recalculates the
    // subtree, and the child keeps rendering/picking with its previous matrix.
    //
    // Expected: after the child moves, the next root-level recalculation pass (the one every
    // frame performs) yields a child matrix that includes the new child position.
    NX::SceneObject parent;
    parent.setPosition(NX::GPos3{ 100.f, 0.f, 0.f });
    auto* child = parent.addChildComponent<NX::SceneObj::Rectangle>();
    parent.tryToRecalculateMatrices();
    ASSERT_FLOAT_EQ(child->getModelMatrix()[3].x, 100.f);

    child->setPosition(NX::GPos3{ 10.f, 0.f, 0.f });
    parent.tryToRecalculateMatrices();

    EXPECT_FLOAT_EQ(child->getModelMatrix()[3].x, 110.f);
}
