// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "Editor/ImGuizmoIntegration.h"
#include "Editor/SceneGizmo.h"
#include "NxWorld/Entities/Camera/Camera.h"
#include "NxWorld/Scene/Rectangle.h"
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

    class GizmoTestCamera : public NX::OrthographicCamera
    {
    public:
        const glm::mat4& getMatrix() override
        {
            _cachedProjMatrix = useEngineDepth ? glm::ortho(0.f, 800.f, 0.f, 600.f, 0.1f, 10000.f)
                                               : glm::ortho(0.f, 800.f, 0.f, 600.f, -10.f, 10.f);
            _cachedModelMatrix = glm::translate(
                glm::mat4(1.f), glm::vec3(0.f, 0.f, useEngineDepth ? -1000.f : 0.f));
            _cachedCalculatedMatrix = _cachedProjMatrix * _cachedModelMatrix;
            return _cachedCalculatedMatrix;
        }

        bool useEngineDepth = false;
    };

    class SceneGizmoInteractionTests : public ::testing::Test
    {
    protected:
        void SetUp() override
        {
            ImGui::CreateContext();
            auto& io = ImGui::GetIO();
            io.IniFilename = nullptr;
            io.DisplaySize = glm::vec2(800.f, 600.f);
            unsigned char* pixels = nullptr;
            int width = 0;
            int height = 0;
            io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
            rectangle.setPosition({ 300.f, 300.f, 0.f });
        }

        void TearDown() override
        {
            ImGuizmo::Enable(false);
            ImGui::DestroyContext();
        }

        void frame(glm::vec2 mouse, bool down)
        {
            auto& io = ImGui::GetIO();
            io.AddMousePosEvent(mouse.x, mouse.y);
            io.AddMouseButtonEvent(ImGuiMouseButton_Left, down);
            ImGui::NewFrame();
            ImGuizmo::BeginFrame();
            ImGui::SetNextWindowPos(glm::vec2(0.f));
            ImGui::SetNextWindowSize(io.DisplaySize);
            ImGui::Begin("Viewport", nullptr, ImGuiWindowFlags_NoDecoration);
            ImGui::Dummy(io.DisplaySize);
            gizmo.draw(rectangle, camera, glm::vec2(0.f), io.DisplaySize);
            ImGui::End();
            ImGui::EndFrame();
        }

        NX::SceneGizmo gizmo;
        NX::SceneObj::Rectangle rectangle;
        GizmoTestCamera camera;
    };

    TEST_F(SceneGizmoInteractionTests, DraggingMoveHandleUpdatesRectangleAndBlocksPicking)
    {
        frame({ 330.f, 300.f }, false);
        frame({ 330.f, 300.f }, false);
        ASSERT_TRUE(gizmo.blocksPicking());
        frame({ 330.f, 300.f }, true);
        frame({ 380.f, 300.f }, true);
        EXPECT_TRUE(gizmo.blocksPicking());
        EXPECT_NEAR(rectangle.getPosition().x, 350.f, 0.1f);
        EXPECT_NEAR(rectangle.getPosition().y, 300.f, 0.1f);
        EXPECT_FLOAT_EQ(rectangle.getPosition().z, 0.f);
        frame({ 380.f, 300.f }, false);
        EXPECT_TRUE(gizmo.blocksPicking());
        frame({ 700.f, 500.f }, false);
        EXPECT_FALSE(gizmo.blocksPicking());
    }
    TEST_F(SceneGizmoInteractionTests, XYPlaneDragPreservesDepthWithEngineClipRange)
    {
        camera.useEngineDepth = true;
        frame({ 335.f, 265.f }, false);
        frame({ 335.f, 265.f }, false);
        frame({ 335.f, 265.f }, true);
        ASSERT_TRUE(ImGuizmo::IsUsing());
        frame({ 385.f, 240.f }, true);
        EXPECT_NEAR(rectangle.getPosition().x, 350.f, 0.1f);
        EXPECT_NEAR(rectangle.getPosition().y, 325.f, 0.1f);
        EXPECT_NEAR(rectangle.getPosition().z, 0.f, 0.001f);
        frame({ 385.f, 240.f }, false);
        EXPECT_NEAR(rectangle.getPosition().z, 0.f, 0.001f);
    }

    TEST_F(SceneGizmoInteractionTests, RotationRingRotatesOnlyAroundZ)
    {
        float radius = 20.f;
        for (; radius < 150.f; radius += 1.f)
        {
            frame({ 300.f - radius, 300.f - radius }, false);
            if (ImGuizmo::IsOver(ImGuizmo::ROTATE_Z))
            {
                break;
            }
        }
        ASSERT_LT(radius, 150.f);
        frame({ 300.f - radius, 300.f - radius }, true);
        frame({ 300.f - radius, 300.f + radius }, true);
        EXPECT_TRUE(gizmo.blocksPicking());
        EXPECT_NEAR(rectangle.getRotationZ(), 90.f, 0.1f);
        EXPECT_FLOAT_EQ(rectangle.getRotationX(), 0.f);
        EXPECT_FLOAT_EQ(rectangle.getRotationY(), 0.f);
        EXPECT_EQ(glm::vec3(rectangle.getPosition()), glm::vec3(300.f, 300.f, 0.f));
        EXPECT_EQ(rectangle.getScale(), glm::vec3(1.f));
        frame({ 300.f - radius, 300.f + radius }, false);
    }
} // namespace

TEST(SceneGizmoTests, TranslationPreservesOriginRotationAndScale)
{
    NX::SceneObj::Rectangle rectangle;
    rectangle.setOrigin({ 50.f, 50.f, 0.f });
    rectangle.setScale({ 2.f, 3.f, 1.f });
    rectangle.setRotationZ(35.f);
    rectangle.recalculateMatrices();
    auto pivot = glm::translate(rectangle.getModelMatrix(), rectangle.getOrigin());
    pivot[3] = glm::vec4(120.f, 85.f, -10.f, 1.f);

    ASSERT_TRUE(NX::SceneGizmo::ApplyTransform(rectangle, pivot, glm::mat4(1.f), false));
    EXPECT_TRUE(rectangle.hasDirtyMatrices());
    EXPECT_EQ(rectangle.getScale(), glm::vec3(2.f, 3.f, 1.f));
    EXPECT_EQ(rectangle.getOrigin(), glm::vec3(50.f, 50.f, 0.f));
    EXPECT_FLOAT_EQ(rectangle.getRotationZ(), 35.f);
    rectangle.recalculateMatrices();
    ExpectMatrixNear(rectangle.getModelMatrix(), glm::translate(pivot, -rectangle.getOrigin()));
}

TEST(SceneGizmoTests, RotationUsesEngineEulerOrderAndPreservesMirroredScale)
{
    NX::SceneObj::Rectangle rectangle;
    rectangle.setOrigin({ 30.f, 45.f, 0.f });
    rectangle.setScale({ -2.f, 3.f, 1.f });

    NX::Transformable expected;
    expected.setPosition({ 100.f, 200.f, -5.f });
    expected.setRotation({ 20.f, -30.f, 65.f });
    expected.setScale(rectangle.getScale());
    expected.setOrigin(rectangle.getOrigin());
    expected.recalculateMatrices();
    const auto pivot = glm::translate(expected.getModelMatrix(), expected.getOrigin());

    ASSERT_TRUE(NX::SceneGizmo::ApplyTransform(rectangle, pivot, glm::mat4(1.f), true));
    rectangle.recalculateMatrices();
    EXPECT_EQ(rectangle.getScale(), expected.getScale());
    ExpectMatrixNear(rectangle.getModelMatrix(), expected.getModelMatrix());
}

TEST(SceneGizmoTests, ConvertsWorldManipulationBackToParentSpace)
{
    NX::SceneObject parent;
    parent.setPosition({ 20.f, 40.f, 0.f });
    parent.setRotationZ(25.f);
    parent.setScale({ 2.f, 2.f, 1.f });
    auto* child = parent.addChildComponent<NX::SceneObj::Rectangle>();
    child->setOrigin({ 50.f, 50.f, 0.f });
    parent.recalculateMatrices();

    NX::Transformable expected;
    expected.setPosition({ 10.f, 30.f, 0.f });
    expected.setRotationZ(70.f);
    expected.setOrigin(child->getOrigin());
    expected.recalculateMatrices(parent.getModelMatrix());
    const auto pivot = glm::translate(expected.getModelMatrix(), expected.getOrigin());

    ASSERT_TRUE(NX::SceneGizmo::ApplyTransform(*child, pivot, parent.getModelMatrix(), true));
    parent.recalculateMatrices();
    ExpectMatrixNear(child->getModelMatrix(), expected.getModelMatrix());
}

TEST(SceneGizmoTests, RejectsSingularTransformsWithoutChangingTheObject)
{
    NX::Transformable object;
    object.setPosition({ 10.f, 20.f, 30.f });
    EXPECT_FALSE(NX::SceneGizmo::ApplyTransform(object, glm::mat4(1.f), glm::mat4(0.f), false));
    EXPECT_EQ(glm::vec3(object.getPosition()), glm::vec3(10.f, 20.f, 30.f));
    object.setScale({ 0.f, 1.f, 1.f });
    EXPECT_FALSE(NX::SceneGizmo::ApplyTransform(object, glm::mat4(1.f), glm::mat4(1.f), true));
    EXPECT_EQ(glm::vec3(object.getPosition()), glm::vec3(10.f, 20.f, 30.f));
}
