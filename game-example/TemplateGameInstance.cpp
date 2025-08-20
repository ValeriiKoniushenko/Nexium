// MIT License
//
// Copyright (c) 2019-2025 Valerii Koniushenko
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "TemplateGameInstance.h"

#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"

using namespace Core;

void TemplateGameInstance::onLoadShaders()
{
    auto* colorShader = shaderManager.getShaderProgram("color"_atom);
    if (Verify(colorShader))
    {
        colorShader->setVertexAttributeCallback(
            []()
            {
                glEnableVertexAttribArray(0);
                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), nullptr);

                glEnableVertexAttribArray(1);
                glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                                      reinterpret_cast<void*>(3 * sizeof(float)));

                glEnableVertexAttribArray(2);
                glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                                      reinterpret_cast<void*>(6 * sizeof(float)));
            });
    }

    auto* gridShader = shaderManager.getShaderProgram("grid"_atom);
    if (Verify(gridShader))
    {
        gridShader->setVertexAttributeCallback(
            []()
            {
            });
    }
}

void TemplateGameInstance::onTick(float delta)
{
    auto* shader = shaderManager.getShaderProgram("color"_atom);
    shader->use();
    shader->setUniform("uObjectColor"_atom, 1.0f, 1.0f, 1.0f);
    shader->setUniform("uLightColor"_atom, 1.0f, 1.0f, 1.0f);
    shader->setUniform("uLightPos"_atom, world.lightPos);
    shader->setUniform("uViewPos"_atom, currentCamera->getPosition());
    shader->setUniform("uTexture"_atom, 0);
    shader->setUniform("uProjAndView"_atom, currentCamera->getMatrix());
    gameScene.directDraw();

    // ================ GRID =================
    auto* gridShader = shaderManager.getShaderProgram("grid"_atom);
    gridShader->use();
    gridShader->setUniform("uProjAndView"_atom, currentCamera->getMatrix());
    gridShader->setUniform("uCameraPos"_atom, currentCamera->getPosition());

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBlendFunc(GL_ONE, GL_ZERO);
    glDisable(GL_BLEND);
}

void TemplateGameInstance::onInitReadCache()
{
    GameInstance::onInitReadCache();
}

void TemplateGameInstance::onFinishWriteCache()
{
    GameInstance::onFinishWriteCache();
}

void TemplateGameInstance::onInitFinish()
{
    std::vector<std::filesystem::path> modelPaths
        = { "assets/base-3d/Models/FBX/Tree.fbx", "assets/base-3d/Models/FBX/FireHydrant.fbx" };

    Assimp::Importer importer;
    for (auto&& path : modelPaths)
    {
        const aiScene* scene = importer.ReadFile(
            path.generic_string().c_str(),
            aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType);
        if (Verify(scene) && Verify(scene->mRootNode))
        {
            StaticMeshBundle mesh;
            mesh.importFrom(scene->mRootNode, scene, path);
            mesh.setShaderProgram(shaderManager.getShaderProgram("color"_atom));
            gameScene.addMesh(std::move(mesh));
        }
    }
}
