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

#include "TemplateGameInstance.h"

#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"

using namespace Core;

void TemplateGameInstance::onLoadShaders()
{
    GameInstance::onLoadShaders();
}

void TemplateGameInstance::onTick(float delta)
{
    gameScene.directDraw();
}

void TemplateGameInstance::onInitializeReadCache()
{
    GameInstance::onInitializeReadCache();
}

void TemplateGameInstance::onFinishWriteCache()
{
    GameInstance::onFinishWriteCache();
}

void TemplateGameInstance::onInitializeFinish()
{
    std::vector modelPaths = { Config::Path::objects3d / "Models/FBX/FireHydrant.fbx",
                               Config::Path::objects3d / "ConiferousForestAssetsPack.glb" };

    Assimp::Importer importer;
    for (auto&& path : modelPaths)
    {
        const aiScene* scene = importer.ReadFile(
            path.generic_string().c_str(),
            aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType
                | aiProcess_PreTransformVertices | aiProcess_GlobalScale);
        if (ASSERT_VAL(scene) && ASSERT_VAL(scene->mRootNode))
        {
            StaticMeshBundle mesh;
            mesh.importFrom(scene->mRootNode, scene, path, 100.f);
            mesh.setShader(shaderManager.getShaderProgram("color"_atom));
            mesh.setOutlineShader(shaderManager.getShaderProgram("outline"_atom));
            gameScene.addActor(std::move(mesh), true);
        }
    }
}
