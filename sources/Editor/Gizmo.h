// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "../Foundation/Configs.h"
#include "../GameplaySystem/Entities/Actor.h"
#include "Core/Delegate.h"
#include "InputDevices/InputAction.h"
#include "NxWorld/Entities/Mesh/StaticMeshBundle.h"
#include "Platform/Window.h"

#include <filesystem>

namespace Core
{
    /// Gizmo class
    /// Deprecated due to unstable behavior
    CLASS();
    class Gizmo : public StaticMeshBundle
    {
        ECS_DECL(Gizmo, Core::StaticMeshBundle);

    public:
        ENUM_CLASS();
        enum class Direction
        {
            X,
            Y,
            Z
        };

        struct DragData : public DragAndDrop::Data
        {
            inline static auto dragType = "gizmo_move"_atom;

            Direction direction;
            std::vector<Transformable*> attachedObjects;
        };

        void pureDraw(const std::function<void(StaticMesh*)>& onUniformSet,
                      const std::function<bool(const Actor*)>& conditional) override;

        void onMousePicked(StaticMesh* clickedPart) override;

        void onTick(float delta) override;

    protected:
        void onDraw(BaseCamera& camera) override;
        void initialize() override;
        void load3DModel();
        void handleDragStart(StaticMesh* touchedMesh);
        void handleDrag();
        void recalculateMatrices(const glm::mat4& mat) override;

    private:
        std::optional<glm::vec3> _lastRay;
    };
} // namespace Core

#include "Gizmo.generated.h" // added by the code generator. Better don't move it.
