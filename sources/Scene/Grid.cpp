/*
 * MIT License
 *
 * Copyright (c) 2018-2027 Valerii Koniushenko
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

#include "Grid.h"

#include "Core/String.h"
#include "GameplaySystem/Framework/GameInstance.h"

namespace Core
{
    void Grid::draw()
    {
        if (!_isDraw)
        {
            return;
        }

        auto* gridShader = GetShaderManager().getShaderProgram("grid"_atom);
        if (Verify(gridShader) && GetWorld().currentCamera)
        {
            gridShader->use();

            gridShader->setUniform("uProjAndView"_atom, GetWorld().currentCamera->getMatrix());
            gridShader->setUniform("uCameraPos"_atom, GetWorld().currentCamera->getPosition());
            gridShader->setUniform("uGlobalGridSize"_atom, _gridSize);
            gridShader->setUniform("uGridCellSize"_atom, _cellSize);

            gridShader->setUniform("uPlaneOrigin"_atom, _origin);
            gridShader->setUniform("uPlaneRight"_atom, _right);
            gridShader->setUniform("uPlaneUp"_atom, _up);
            // gridShader->setUniform("uPlaneNormal"_atom, _normal);

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
        glm::vec3 helper = (std::abs(_normal.y) > 0.999f) ? glm::vec3(1, 0, 0) : glm::vec3(0, 1, 0);
        _right = glm::normalize(glm::cross(helper, _normal));
        _up = glm::normalize(glm::cross(_normal, _right));
    }

} // namespace Core