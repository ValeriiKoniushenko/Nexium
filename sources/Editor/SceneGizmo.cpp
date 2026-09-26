// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "SceneGizmo.h"

#include "ImGuizmoIntegration.h"
#include "NxWorld/Entities/Camera/Camera.h"
#include "NxWorld/Scene/Rectangle.h"
#include "glm/gtc/type_ptr.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/euler_angles.hpp"

namespace NX
{
    namespace
    {
        void SetupGizmoStyle()
        {
            const float scale = ImGui::GetFontSize() / 16.f;
            auto& style = ImGuizmo::GetStyle();
            style.TranslationLineThickness = 3.f * scale;
            style.TranslationLineArrowSize = 9.f * scale;
            style.RotationLineThickness = 2.5f * scale;
            style.RotationOuterLineThickness = 3.5f * scale;
            style.ScaleLineThickness = 3.f * scale;
            style.ScaleLineCircleSize = 7.f * scale;
            style.HatchedAxisLineThickness = 4.f * scale;
            style.CenterCircleSize = 7.5f * scale;

            style.Colors[ImGuizmo::DIRECTION_X] = { 0.95f, 0.28f, 0.32f, 1.f };
            style.Colors[ImGuizmo::DIRECTION_Y] = { 0.35f, 0.82f, 0.47f, 1.f };
            style.Colors[ImGuizmo::DIRECTION_Z] = { 0.30f, 0.58f, 0.95f, 1.f };
            style.Colors[ImGuizmo::PLANE_X] = { 0.95f, 0.28f, 0.32f, 0.24f };
            style.Colors[ImGuizmo::PLANE_Y] = { 0.35f, 0.82f, 0.47f, 0.24f };
            style.Colors[ImGuizmo::PLANE_Z] = { 0.30f, 0.58f, 0.95f, 0.24f };
            style.Colors[ImGuizmo::SELECTION] = { 0.f, 0.88f, 0.77f, 1.f };
            style.Colors[ImGuizmo::INACTIVE] = { 0.5f, 0.52f, 0.62f, 0.65f };
            style.Colors[ImGuizmo::TRANSLATION_LINE] = { 0.82f, 0.84f, 0.9f, 0.85f };
            style.Colors[ImGuizmo::SCALE_LINE] = { 0.5f, 0.52f, 0.62f, 1.f };
            style.Colors[ImGuizmo::ROTATION_USING_BORDER] = { 0.f, 0.88f, 0.77f, 1.f };
            style.Colors[ImGuizmo::ROTATION_USING_FILL] = { 0.f, 0.88f, 0.77f, 0.18f };
            style.Colors[ImGuizmo::HATCHED_AXIS_LINES] = { 0.08f, 0.09f, 0.12f, 0.55f };
            style.Colors[ImGuizmo::TEXT] = { 0.9f, 0.9f, 0.95f, 1.f };
            style.Colors[ImGuizmo::TEXT_SHADOW] = { 0.f, 0.f, 0.f, 0.8f };
        }
    } // namespace

    bool SceneGizmo::ApplyTransform(Transformable& object, const glm::mat4& worldPivot,
                                    const glm::mat4& parent, bool rotate)
    {
        const auto scale = object.getScale();
        if (glm::abs(glm::determinant(parent)) < 1.e-6f
            || glm::any(glm::lessThan(glm::abs(scale), glm::vec3(1.e-6f))))
        {
            return false;
        }

        auto local = glm::inverse(parent) * worldPivot;
        object.setPosition(GPos3(glm::vec3(local[3])));
        if (rotate)
        {
            // Nexium composes rotations as Rx * Ry * Rz, unlike ImGuizmo's Euler helper.
            for (int axis = 0; axis < 3; ++axis)
            {
                local[axis] /= scale[axis];
            }
            glm::vec3 angles;
            glm::extractEulerAngleXYZ(local, angles.x, angles.y, angles.z);
            object.setRotation(glm::degrees(angles));
        }
        return true;
    }

    void SceneGizmo::draw(SceneObject& object, BaseCamera& camera, glm::vec2 position,
                          glm::vec2 size)
    {
        const bool wasUsing = ImGuizmo::IsUsing();
        glm::mat4 parent(1.f);
        auto* root = &object;
        for (auto* ancestor = object.getParent(); ancestor; ancestor = ancestor->getParent())
        {
            if (auto* sceneObject = dynamic_cast<SceneObject*>(ancestor))
            {
                root = sceneObject;
            }
        }
        root->recalculateMatrices();
        if (auto* transform = dynamic_cast<Transformable*>(object.getParent()))
        {
            parent = transform->getModelMatrix();
        }

        // Move the gizmo to the object's pivot, retaining its origin offset in the model.
        auto matrix = glm::translate(object.getModelMatrix(), object.getOrigin());
        const auto clipPivot = camera.getMatrix() * matrix[3];
        if (glm::abs(glm::determinant(matrix)) < 1.e-6f
            || (camera.getType() == CameraType::Perspective && clipPivot.z < 0.001f))
        {
            ImGuizmo::Enable(false);
            _blocksPicking = wasUsing;
            return;
        }
        const bool rectangle = dynamic_cast<SceneObj::Rectangle*>(&object) != nullptr;
        const auto operation
            = rectangle ? ImGuizmo::TRANSLATE_X | ImGuizmo::TRANSLATE_Y | ImGuizmo::ROTATE_Z
                        : ImGuizmo::TRANSLATE | ImGuizmo::ROTATE;

        ImGuizmo::SetDrawlist(ImGui::GetWindowDrawList());
        ImGuizmo::SetRect(position.x, position.y, size.x, size.y);
        ImGuizmo::SetOrthographic(camera.getType() == CameraType::Orthographic);
        ImGuizmo::SetGizmoSizeClipSpace(0.15f);
        SetupGizmoStyle();
        ImGuizmo::Enable(true);
        // ImGuizmo unprojects depth in [0, 1]; the renderer uses OpenGL's [-1, 1].
        auto gizmoProjection = camera.getCachedProjectionMatrix();
        for (int column = 0; column < 4; ++column)
        {
            gizmoProjection[column][2]
                = (gizmoProjection[column][2] + gizmoProjection[column][3]) * 0.5f;
        }
        if (ImGuizmo::Manipulate(glm::value_ptr(camera.getModelMatrix()),
                                 glm::value_ptr(gizmoProjection), operation, ImGuizmo::WORLD,
                                 glm::value_ptr(matrix)))
        {
            ApplyTransform(object, matrix, parent, true);
            root->recalculateMatrices();
        }
        _blocksPicking = wasUsing || ImGuizmo::IsOver() || ImGuizmo::IsUsing();
    }
} // namespace NX
