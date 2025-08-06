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

void TemplateGameInstance::onLoadShaders()
{
    auto* shader = _shaderManager->getShaderProgram("color"_atom);
    if (Verify(shader))
    {
        shader->setVertexAttributeCallback(
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
}

void TemplateGameInstance::onTick(float delta)
{
    auto* shader = _shaderManager->getShaderProgram("color"_atom);
    shader->use();
    shader->setUniform("uObjectColor"_atom, 1.0f, 1.0f, 1.0f);
    shader->setUniform("uLightColor"_atom, 1.0f, 1.0f, 1.0f);
    shader->setUniform("uLightPos"_atom, world.lightPos);
    shader->setUniform("uViewPos"_atom, camera.getPosition());
    shader->setUniform("uTexture"_atom, 0);
    shader->setUniform("uProjAndView"_atom, camera.getMatrix());

    for (auto&& m : meshes)
    {
        m.directDraw();
    }
}

void TemplateGameInstance::onInitFinish()
{
    camera.moveForward(-100);
    camera.setFov(90.f);

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
            SW::StaticMeshBundle mesh;
            mesh.importFrom(scene->mRootNode, scene, path);
            mesh.setShaderProgram(_shaderManager->getShaderProgram("color"_atom));
            meshes.push_back(std::move(mesh));
        }
    }

    SW::StaticMesh* targetMesh = nullptr;

    for (auto& mesh : meshes)
    {
        int counter = 0;
        mesh.forEach(
            [&counter, &targetMesh](SW::BaseComponent* c)
            {
                // just stupid code to disable useless nodes\meshes
                if (c->getComponentName().find("FireHydrant_LOD0"))
                {
                    targetMesh = c->castTo<SW::StaticMesh>();
                }

                if (!c->getComponentName().find("_LOD0"))
                {
                    c->setEnabled(false);
                }

                if (c->getComponentName() == "Tree_LOD0")
                {
                    ++counter;
                    if (counter == 2)
                    {
                        auto* m = dynamic_cast<SW::StaticMesh*>(c);
                        m->setDrawModifiers(
                            { { GL_CULL_FACE, SW::GraphicsComponentData::Modifier::Disable } });
                    }
                }
            });
    }

    if (auto* wnd = gameEditor.getWindow<SW::ObjectPropertiesWindow>("Actor properties"))
    {
        wnd->setTargetActor(targetMesh);
    }
}
