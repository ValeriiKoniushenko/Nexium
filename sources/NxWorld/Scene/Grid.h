// Nexium
// Copyright 2018-2027 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "Core/Color.h"

namespace Core
{
    class Grid
    {
    public:
        [[nodiscard]] float getGridSize() const noexcept { return _gridSize; }
        [[nodiscard]] float getCellSize() const noexcept { return _cellSize; }
        void setGridSize(float value) noexcept { _gridSize = value; }
        void setCellSize(float value) noexcept { _cellSize = value; }

        [[nodiscard]] bool isDraw() const noexcept { return _isDraw; }
        void setIsDraw(bool value) noexcept { _isDraw = value; }

        void draw();

        void setPlane(const glm::vec3& origin, const glm::vec3& normal);
        void rebuildBasis();

    protected:
        glm::vec3 _origin = glm::vec3(0.0f);
        glm::vec3 _normal = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::vec3 _right = glm::vec3(1.0f, 0.0f, 0.0f);
        glm::vec3 _up = glm::vec3(0.0f, 1.0f, 0.0f);

        float _gridSize = 10'000.f;
        float _cellSize = 100.f;
        bool _isDraw = true;
    };
} // namespace Core