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

#include "StaticMeshBundle.h"

#include "Editor/Configs.h"
#include "Graphics/Image.h"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"

#include <Core/Timer.h>

using namespace Core;

namespace
{
    void recursiveImportFrom(BaseComponent* rootComponent, const aiNode* node, const aiScene* scene,
                             const std::filesystem::path& modelPath, float scale)
    {
        for (uint32_t i = 0; i < node->mNumMeshes; ++i)
        {
            auto* topMesh = rootComponent->addChildComponent<StaticMesh>();

            const aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            topMesh->setComponentName(mesh->mName.C_Str());
            topMesh->importFrom(mesh, scene, modelPath, scale);
        }

        for (uint32_t i = 0; i < node->mNumChildren; ++i)
        {
            recursiveImportFrom(rootComponent, node->mChildren[i], scene, modelPath, scale);
        }
    }
} // namespace

namespace Core
{
    ECS_COMPONENT_IMPL(StaticMeshBundle)

    StaticMeshBundle::~StaticMeshBundle()
    {
        privateClear();
    }

    StaticMeshBundle::StaticMeshBundle(const StaticMeshBundle& other)
        : Actor(componentType, other._name)
    {
        *this = other;
    }

    StaticMeshBundle& StaticMeshBundle::operator=(const StaticMeshBundle& other)
    {
        if (this != &other)
        {
            Actor::operator=(other);
            _ignoreSelect = other._ignoreSelect;

            invalidateFastAccessContainers();
        }

        return *this;
    }

    void StaticMeshBundle::draw()
    {
        if (!_isEnabled)
        {
            return;
        }

        tryToRecalculateMatrices();

        for (auto* mesh : _meshes)
        {
            if (mesh->isEnabled())
            {
                mesh->draw();
            }
        }

        for (auto* mesh : _bundles)
        {
            if (mesh->isEnabled())
            {
                mesh->draw();
            }
        }
    }

    void StaticMeshBundle::importFrom(const aiNode* node, const aiScene* scene,
                                      const std::filesystem::path& modelPath,
                                      float scale /* = 1.f*/)
    {
        if (!DEBUG_ASSERT_VAL(node))
        {
            errorLog("Can't import model[s] for static mesh bundle, so node is nullptr");
            return;
        }
        if (!DEBUG_ASSERT_VAL(scene))
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
        recursiveImportFrom(this, node, scene, modelPath, scale);

        globalLog.debugLog("MeshBundle '{}' was loaded to the world."_f << _name);
    }

    void StaticMeshBundle::setShader(ShaderProgram* sp, bool ignoreVertexAttribSetup /* = false*/)
    {
        if (!DEBUG_ASSERT_VAL(sp))
        {
            return;
        }

        for (auto* mesh : _meshes)
        {
            if (DEBUG_ASSERT_VAL(mesh)) [[likely]]
            {
                mesh->setShader(sp, ignoreVertexAttribSetup);
            }
        }
    }

    void StaticMeshBundle::clear()
    {
        privateClear();
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

    std::size_t StaticMeshBundle::getRenderableTriangles() const
    {
        std::size_t out = 0;
        for (const auto* mesh : _meshes)
        {
            if (mesh->isEnabled())
            {
                out += mesh->getTriangleCount();
            }
        }
        for (const auto* bundle : _bundles)
        {
            if (bundle->isEnabled() && !bundle->isPostDraw())
            {
                out += bundle->getRenderBundlesCount();
            }
        }

        return out;
    }

    StringAtom StaticMeshBundle::getCacheHash() const
    {
        return getComponentName();
    }

    nlohmann::json StaticMeshBundle::toCacheData() const
    {
        return toJson();
    }
    std::filesystem::path StaticMeshBundle::getCacheDir() const
    {
        return JsonCacheable::getCacheDir() / Config::Path::gameConfigModel3DsDir;
    }

    void StaticMeshBundle::fromCacheData(const nlohmann::json& data)
    {
        fromJson(data, false);
    }

    void StaticMeshBundle::onAddChild(BaseComponent* newChild)
    {
        Actor::onAddChild(newChild);
        if (auto* mesh = newChild->tryCastTo<StaticMesh>())
        {
            _meshes.push_back(mesh);
        }

        if (auto* mesh = newChild->tryCastTo<StaticMeshBundle>())
        {
            _bundles.push_back(mesh);
        }

        setDirtyMatrices();
    }

    void StaticMeshBundle::onRemoveChild(BaseComponent* child)
    {
        Actor::onRemoveChild(child);

        if (auto* mesh = child->tryCastTo<StaticMesh>())
        {
            if (const auto it = std::ranges::find(_meshes, mesh); it != _meshes.end())
            {
                _meshes.erase(it);
            }
        }

        if (auto* mesh = child->tryCastTo<StaticMeshBundle>())
        {
            if (const auto it = std::ranges::find(_bundles, mesh); it != _bundles.end())
            {
                _bundles.erase(it);
            }
        }

        setDirtyMatrices();
    }

    void StaticMeshBundle::invalidateFastAccessContainers()
    {
        _meshes.clear();
        _bundles.clear();

        for (auto& child : _children)
        {
            if (auto* mesh = child->tryCastTo<StaticMesh>())
            {
                _meshes.push_back(mesh);
            }
            else if (auto* bundle = child->tryCastTo<StaticMeshBundle>())
            {
                bundle->invalidateFastAccessContainers();
                _bundles.push_back(bundle);
            }
        }
    }

    void StaticMeshBundle::privateClear()
    {
        BaseComponent::clear();

        clearMeshes();
    }

    void StaticMeshBundle::setOutlineShader(ShaderProgram* sp, bool ignoreVertexAttribSetup)
    {
        for (auto* mesh : _meshes)
        {
            if (DEBUG_ASSERT_VAL(mesh)) [[likely]]
            {
                mesh->setOutlineShader(sp, ignoreVertexAttribSetup);
            }
        }
    }

    void StaticMeshBundle::onTick(float delta)
    {
        Actor::onTick(delta);
    }

    void StaticMeshBundle::pureDraw(const std::function<void(StaticMesh*)>& onUniformSet,

                                    const std::function<bool(const Actor*)>& conditional)
    {
        if (!_isEnabled)
        {
            return;
        }

        bool isDraw = true;
        if (conditional && !conditional(this))
        {
            isDraw = false;
        }

        tryToRecalculateMatrices();

        if (isDraw)
        {
            for (auto* mesh : _meshes)
            {
                if (mesh->isEnabled())
                {
                    mesh->pureDraw(onUniformSet);
                }
            }
        }

        for (auto* mesh : _bundles)
        {
            if (mesh->isEnabled())
            {
                mesh->pureDraw(onUniformSet, conditional);
            }
        }
    }

    void StaticMeshBundle::recalculateMatrices(const glm::mat4& mat)
    {
        Actor::recalculateMatrices(mat);

        for (auto&& comp : _children)
        {
            if (auto* bundle = dynamic_cast<StaticMeshBundle*>(comp.get()))
            {
                bundle->recalculateMatrices(_cachedModelMatrix);
            }
            else if (auto* mesh = dynamic_cast<StaticMesh*>(comp.get()))
            {
                mesh->recalculateMatrices(_cachedModelMatrix);
            }
            else if (auto* trans = dynamic_cast<Transformable*>(comp.get()))
            {
                trans->recalculateMatrices(_cachedModelMatrix);
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