// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "Grid.h"

#include "Core/String.h"
#include "NxWorld/Framework/GameInstance.h"

namespace Core
{
    void Grid::draw()
    {
        if (!_isDraw)
        {
            return;
        }

        static const GLuint gridVAO = []()
        {
            GLuint vao = 0;
            glGenVertexArrays(1, &vao);
            return vao;
        }();

        auto* gridShader = GetShaderManager().getShaderProgram("grid"_atom);
        if (Verify(gridShader) && GetWorld()->currentCamera)
        {
            glBindVertexArray(gridVAO);

            gridShader->use();

            gridShader->setUniform("uProjAndView"_atom, GetWorld()->currentCamera->getMatrix());
            gridShader->setUniform("uCameraPos"_atom, GetWorld()->currentCamera->getPosition());
            gridShader->setUniform("uGlobalGridSize"_atom, _gridSize);
            gridShader->setUniform("uGridCellSize"_atom, _cellSize);

            gridShader->setUniform("uPlaneOrigin"_atom, _origin);
            gridShader->setUniform("uPlaneRight"_atom, _right);
            gridShader->setUniform("uPlaneUp"_atom, _up);

            glDisable(GL_CULL_FACE);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            glDrawArrays(GL_TRIANGLES, 0, 6);

            glBlendFunc(GL_ONE, GL_ZERO);
            glEnable(GL_CULL_FACE);
        }
    }

    void Grid::setPlane(const glm::vec3& origin, const glm::vec3& normal)
    {
        _origin = origin;
        _normal = glm::normalize(normal);
        rebuildBasis();
    }

    void Grid::rebuildBasis()
    {
        const glm::vec3 helper
            = (std::abs(_normal.y) > 0.999f) ? glm::vec3(1, 0, 0) : glm::vec3(0, 1, 0);
        _right = glm::normalize(glm::cross(helper, _normal));
        _up = glm::normalize(glm::cross(_normal, _right));
    }

} // namespace Core
