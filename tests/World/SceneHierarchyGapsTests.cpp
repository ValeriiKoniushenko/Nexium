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

#include "NxWorld/Entities/Camera/Camera.h"
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

namespace
{
    class CountingDrawable : public NX::SceneObject
    {
    public:
        int draws = 0;

    protected:
        void onDraw(NX::BaseCamera&) override { ++draws; }
    };
} // namespace

TEST(SceneHierarchyGaps, DrawingAParentDrawsItsDrawableChildren)
{
    // The editor lets any SceneObject (Rectangle included) be attached as a child of another
    // scene object, and the hierarchy already propagates transforms to children
    // (SceneObject::recalculateMatrices) and ticks them (BaseComponent::onTick). Rendering does
    // not follow: Scene::directDraw iterates only top-level objects and SceneObject::draw is
    // final and never visits children, so a sprite parented to another object is silently
    // invisible (and RectangleBasedObjectPicker never considers it either).
    //
    // Expected: drawing the parent draws its enabled drawable children, in hierarchy order.
    NX::OrthographicCamera camera;
    NX::SceneObject parent;
    auto* child = parent.addChildComponent<CountingDrawable>();
    auto* grandChild = child->addChildComponent<CountingDrawable>();

    parent.draw(camera);

    EXPECT_EQ(child->draws, 1);
    EXPECT_EQ(grandChild->draws, 1);
}

TEST(SceneHierarchyGaps, DisabledChildIsNotDrawnEvenIfParentIs)
{
    // Counterpart of the test above: isEnabled() already gates SceneObject::draw for the object
    // itself; a child's own flag must gate the child.
    NX::OrthographicCamera camera;
    NX::SceneObject parent;
    auto* child = parent.addChildComponent<CountingDrawable>();
    child->setEnabled(false);

    parent.draw(camera);

    EXPECT_EQ(child->draws, 0);
}

TEST(SceneHierarchyGaps, GlobalPositionFollowsTheParentsRotationAndScale)
{
    // SceneObject::getGlobalPosition()/getGlobalRotation() (and BaseCamera::recalculate-
    // CameraMatrices, which repeats the same walk) add up the parent chain's positions and
    // Euler angles. That is only correct while every ancestor is unrotated and unscaled: a
    // child at local (100, 0, 0) under a parent rotated 90 degrees around Z sits at world
    // (0, 100, 0), which is what the model matrix (and the rendered sprite) says. The Spectator
    // -> OrthographicCamera rig is exactly such a parent/child pair, so a rotated or scaled
    // spectator would move its camera somewhere else than its gizmo shows. For 3D parents
    // Euler angles do not even add. Expected to FAIL until global transforms are derived from
    // the composed model matrix.
    NX::SceneObject parent;
    parent.setRotationZ(90.f);
    parent.setScale({ 2.f, 2.f, 1.f });
    auto* child = parent.addChildComponent<NX::SceneObj::Rectangle>();
    child->setPosition(NX::GPos3{ 100.f, 0.f, 0.f });
    parent.recalculateMatrices();

    const glm::vec3 fromMatrix = child->getModelMatrix()[3];
    const glm::vec3 reported = child->getGlobalPosition();
    EXPECT_NEAR(reported.x, fromMatrix.x, 1.e-3f);
    EXPECT_NEAR(reported.y, fromMatrix.y, 1.e-3f);
    EXPECT_NEAR(fromMatrix.y, 200.f, 1.e-3f); // sanity: the matrix really rotates and scales
}
