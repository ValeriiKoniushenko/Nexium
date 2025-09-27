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
        if (ASSERT_VAL(gridShader) && gGameInstance->currentCamera)
        {
            gridShader->use();

            gridShader->setUniform("uProjAndView"_atom, gGameInstance->currentCamera->getMatrix());
            gridShader->setUniform("uCameraPos"_atom, gGameInstance->currentCamera->getPosition());
            gridShader->setUniform("uGlobalGridSize"_atom, _gridSize);
            /*gridShader->setUniform("uMinimalDistanceBetweenPixels"_atom,
                                   _minimalDistanceBetweenPixels);*/
            gridShader->setUniform("uGridCellSize"_atom, _cellSize);
            // gridShader->setUniform("uGridColorThin"_atom, _gridColorThin);
            // gridShader->setUniform("uGridColorThick"_atom, _gridColorThick);

            glEnable(GL_BLEND);
            glDisable(GL_CULL_FACE);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            glDrawArrays(GL_TRIANGLES, 0, 6);

            glBlendFunc(GL_ONE, GL_ZERO);
            glEnable(GL_CULL_FACE);
            glDisable(GL_BLEND);
        }
    }
} // namespace Core