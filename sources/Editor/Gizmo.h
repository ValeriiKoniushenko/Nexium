// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "Core/Delegate.h"
#include "Foundation/Configs.h"
#include "NxSubsystems/Input/InputAction.h"
#include "NxWorld/Entities/Actor.h"
#include "NxWorld/Entities/Mesh/StaticMeshBundle.h"
#include "Platform/Window.h"

#include <filesystem>

namespace NX
{
    /// Gizmo class
    /// Deprecated due to unstable behavior
    CLASS();
    class Gizmo : public NX::StaticMeshBundle
    {
        ECS_DECL(Gizmo, NX::StaticMeshBundle);

    public:
        ENUM_CLASS();
        enum class Direction
        {
            X,
            Y,
            Z
        };

        struct DragData : public Platform::DragAndDrop::Data
        {
            inline static auto dragType = "gizmo_move"_atom;

            Direction direction;
            std::vector<NX::Transformable*> attachedObjects;
        };

        void pureDraw(const std::function<void(NX::StaticMesh*)>& onUniformSet,
                      const std::function<bool(const NX::Actor*)>& conditional) override;

        void onMousePicked(NX::StaticMesh* clickedPart) override;

        void onTick(float delta) override;

    protected:
        void onDraw(NX::BaseCamera& camera) override;
        void initialize() override;
        void load3DModel();
        void handleDragStart(NX::StaticMesh* touchedMesh);
        void handleDrag();
        void recalculateMatrices(const glm::mat4& mat) override;

    private:
        std::optional<glm::vec3> _lastRay;
    };
} // namespace NX

#include "Gizmo.generated.h" // added by the code generator. Better don't move it.
