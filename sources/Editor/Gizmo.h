/*
 * MIT License
 *
 * Copyright (c) 2018-2025 Valerii Koniushenko
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#pragma once

#include "../Misc/Configs.h"
#include "Core/Delegate.h"
#include "GameplaySystem/Framework/Actor.h"
#include "Graphics/Primitives/StaticMeshBundle.h"
#include "Graphics/Window.h"
#include "InputDevices/InputAction.h"

#include <filesystem>

namespace Core
{
    class Gizmo : public StaticMeshBundle
    {
        ECS_COMPONENT_DECL(Gizmo, StaticMeshBundle);

    public:
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

        void draw() override;

        void pureDraw(const std::function<void(StaticMesh*)>& onUniformSet,
                      const std::function<bool(const Actor*)>& conditional) override;

        void onMousePicked(StaticMesh* clickedPart) override;

        void onTick(float delta) override;

    protected:
        void initialize() override;

        void load3DModel();

        void handleDragStart(StaticMesh* touchedMesh);

        void handleDrag();

    private:
        std::optional<glm::vec3> _lastRay;
    };
} // namespace Core