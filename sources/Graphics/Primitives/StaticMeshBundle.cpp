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

#include "StaticMeshBundle.h"

#include "Graphics/Image.h"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"

#include <Core/Timer.h>

namespace
{
    void recursiveImportFrom(SW::BaseComponent* rootComponent, const aiNode* node,
                             const aiScene* scene, const std::filesystem::path& modelPath,
                             SW::StaticMeshBundle::MeshesT& container)
    {
        for (uint32_t i = 0; i < node->mNumMeshes; ++i)
        {
            auto* topMesh = rootComponent->addChildComponent<SW::StaticMesh>();
            topMesh->setComponentName(node->mName.C_Str());

            const auto meshIndex = node->mMeshes[i];
            topMesh->importFrom(scene->mMeshes[meshIndex], scene, modelPath);
            container.push_back(topMesh);
        }

        for (uint32_t i = 0; i < node->mNumChildren; ++i)
        {
            recursiveImportFrom(rootComponent, node->mChildren[i], scene, modelPath, container);
        }
    }
} // namespace

namespace SW
{

    void StaticMeshBundle::directDraw()
    {
        for (auto* mesh : _meshes)
        {
            if (Verify(mesh)) [[likely]]
            {
                mesh->directDraw();
            }
        }
    }

    void StaticMeshBundle::importFrom(const aiNode* node, const aiScene* scene,
                                      const std::filesystem::path& modelPath)
    {
        if (!Verify(node))
        {
            errorLog("Can't import model[s] for static mesh bundle, so node is nullptr");
            return;
        }
        if (!Verify(scene))
        {
            errorLog("Can't import model[s] for static mesh bundle, so scene is nullptr");
            return;
        }

        clearMeshes();

        if (node->mNumMeshes == 0 && node->mNumChildren == 0)
        {
            return;
        }

        setComponentName(modelPath.stem().c_str());
        recursiveImportFrom(this, node, scene, modelPath, _meshes);

        ComponentIterator it(this);
        ComponentIterator it2(this);
        it2.swap(it);
        it2 == it;
        it2 != it;
        (*it)->getComponentType();
        it->clear();
    }

    void StaticMeshBundle::setShaderProgram(ShaderProgram* sp,
                                            bool ignoreVertexAttribSetup /* = false*/)
    {
        for (auto* mesh : _meshes)
        {
            if (Verify(mesh)) [[likely]]
            {
                mesh->setShaderProgram(sp, ignoreVertexAttribSetup);
            }
        }
    }

    void StaticMeshBundle::clear()
    {
        BaseComponent::clear();

        clearMeshes();
    }

    void StaticMeshBundle::clearMeshes()
    {
        _meshes.clear();

        for (auto i = _children.begin(); i != _children.end();)
        {
            if ((*i)->isTypeOf<StaticMesh>())
            {
                i = _children.erase(i);
            }
            else
            {
                ++i;
            }
        }
    }

} // namespace SW