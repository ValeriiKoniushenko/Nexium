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

using namespace Core;

namespace
{
    void recursiveImportFrom(BaseComponent* rootComponent, const aiNode* node, const aiScene* scene,
                             const std::filesystem::path& modelPath,
                             StaticMeshBundle::MeshesT& container)
    {
        for (uint32_t i = 0; i < node->mNumMeshes; ++i)
        {
            auto* topMesh = rootComponent->addChildComponent<StaticMesh>();

            const aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            topMesh->setComponentName(mesh->mName.C_Str());
            topMesh->importFrom(mesh, scene, modelPath);

            container.push_back(topMesh);
        }

        for (uint32_t i = 0; i < node->mNumChildren; ++i)
        {
            recursiveImportFrom(rootComponent, node->mChildren[i], scene, modelPath, container);
        }
    }
} // namespace

namespace Core
{

    ECS_REGISTER_NEW_COMPONENT_TYPE(StaticMeshBundle)

    void StaticMeshBundle::draw()
    {
        if (!_isEnabled)
        {
            return;
        }

        if (_isDirtyModelMatrix)
        {
            recalculateMatrices();
            for (auto* m : _meshes)
            {
                m->setDirtyMatrices();
            }
        }

        for (auto&& comp : _children)
        {
            if (auto* mesh = comp->tryCastTo<StaticMesh>(); mesh && mesh->isEnabled())
            {
                mesh->tryToRecalculateMatrices(_cachedModelMatrix);
                mesh->draw();
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

        setComponentName(modelPath.stem().generic_string().c_str());
        recursiveImportFrom(this, node, scene, modelPath, _meshes);
    }

    void StaticMeshBundle::setShader(ShaderProgram* sp,
                                            bool ignoreVertexAttribSetup /* = false*/)
    {
        for (auto* mesh : _meshes)
        {
            if (Verify(mesh)) [[likely]]
            {
                mesh->setShader(sp, ignoreVertexAttribSetup);
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

    nlohmann::json StaticMeshBundle::toJson() const
    {
        auto json = Actor::toJson();

        for (const auto* m : _meshes)
        {
            json["meshes"].push_back(m->toJson());
        }

        return json;
    }

    void StaticMeshBundle::fromJson(const nlohmann::json& json, bool isIgnoreChildren /* = false*/)
    {
        Actor::fromJson(json, true);

        if (json.contains("meshes"))
        {
            nlohmann::json::array_t arr = json["meshes"];
            if (_meshes.size() != arr.size())
            {
                return;
            }

            for (std::size_t i = 0; i < arr.size(); ++i)
            {
                _meshes.at(i)->fromJson(arr[i], false);
            }
        }
    }

    StringAtom StaticMeshBundle::getCacheHash() const
    {
        return getComponentName();
    }

    nlohmann::json StaticMeshBundle::toCacheData() const
    {
        return toJson();
    }

    void StaticMeshBundle::fromCacheData(const nlohmann::json& data)
    {
        fromJson(data, false);
    }

    void StaticMeshBundle::setOutlineShader(ShaderProgram* sp, bool ignoreVertexAttribSetup)
    {
        for (auto* mesh : _meshes)
        {
            if (Verify(mesh)) [[likely]]
            {
                mesh->setOutlineShader(sp, ignoreVertexAttribSetup);
            }
        }
    }

    void StaticMeshBundle::pureDraw()
    {
        if (!_isEnabled)
        {
            return;
        }

        if (_isDirtyModelMatrix)
        {
            recalculateMatrices();
            for (auto* m : _meshes)
            {
                m->setDirtyMatrices();
            }
        }

        for (auto&& comp : _children)
        {
            if (auto* mesh = comp->tryCastTo<StaticMesh>(); mesh && mesh->isEnabled())
            {
                mesh->tryToRecalculateMatrices(_cachedModelMatrix);
                mesh->pureDraw();
            }
        }
    }

    void StaticMeshBundle::tryToRecalculateMatrices()
    {
        if (_isDirtyModelMatrix)
        {
            recalculateMatrices();
            for (auto* m : _meshes)
            {
                m->setDirtyMatrices();
            }

            for (auto&& comp : _children)
            {
                if (auto* mesh = comp->tryCastTo<StaticMesh>(); mesh && mesh->isEnabled())
                {
                    mesh->tryToRecalculateMatrices(_cachedModelMatrix);
                }
            }
        }
    }

    void StaticMeshBundle::onOutlineStatusChange(bool newStatus)
    {
        for (auto&& mesh : _meshes)
        {
            if (mesh->isEnabled())
            {
                mesh->setIsDrawOutline(newStatus);
            }
        }
    }

} // namespace Core