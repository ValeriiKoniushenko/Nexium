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

#include "StaticMesh.h"

#include "Graphics/Image.h"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"

#include <Core/Timer.h>

namespace SW
{

    ECS_REGISTER_NEW_COMPONENT_TYPE(StaticMesh)

    void StaticMesh::importFrom(const aiMesh* rawMesh, const aiScene* scene,
                                const std::filesystem::path& modelPath /* = ""*/)
    {
        if (!Verify(rawMesh)) [[unlikely]]
        {
            errorLog("Impossible to import nullptr aiMesh*");
            return;
        }

        generate();

        calculateSizeBaseOnMesh(rawMesh, {});
        aiMaterial* material = scene->mMaterials[rawMesh->mMaterialIndex];
        aiString texturePath;
        if (material->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath) == AI_SUCCESS)
        {
            const auto relative = Core::StringAtom(texturePath.C_Str()).replaceAll("\\", "/");
            const auto resolved
                = (modelPath.parent_path() / relative.toStdString()).lexically_normal();

            Image image;
            if (Verify(image.loadImageFromFile(resolved, true)))
            {
                setTexture(image.data(), image.getSize().width, image.getSize().height,
                           image.getChannelAsOpenGLType());
            }
        }

        setMesh(rawMesh, true, true);
    }

    nlohmann::json StaticMesh::toJson() const
    {
        nlohmann::json json;

        json["Transformable"] = Transformable::toJson();
        json["BaseComponent"] = BaseComponent::toJson();
        json["GraphicsComponentData"] = GraphicsComponentData::toJson();

        return json;
    }

    void StaticMesh::fromJson(const nlohmann::json& json, bool isIgnoreChildren /* = false*/)
    {
        Transformable::fromJson(json["Transformable"], isIgnoreChildren);
        BaseComponent::fromJson(json["BaseComponent"], isIgnoreChildren);
        GraphicsComponentData::fromJson(json["GraphicsComponentData"], isIgnoreChildren);
    }

    void StaticMesh::draw()
    {
        if (!GraphicsComponentData::isValid()) [[unlikely]]
        {
            Assert("Can't draw graphic component. It wasn't configured.");
            return;
        }

        for (auto [val, mod] : _drawModifiers)
        {
            if (mod.cast() == Modifier::Enable)
            {
                glEnable(val);
            }
            else if (mod.cast() == Modifier::Disable)
            {
                glDisable(val);
            }
        }

        _shader->use();
        glBindVertexArray(_vao);
        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, _texture);

        applyUniforms();

        glDrawElements(GL_TRIANGLES, _triangleCount, GL_UNSIGNED_INT, 0);

        for (auto [val, mod] : _drawModifiers)
        {
            if (mod.cast() == Modifier::Disable)
            {
                glEnable(val);
            }
            else if (mod.cast() == Modifier::Enable)
            {
                glDisable(val);
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

    void StaticMesh::applyUniforms()
    {
        GraphicsComponentData::applyUniforms();

        tryToRecalculateMatrices();
        _shader->setUniform("uModel"_atom, _cachedModelMatrix);
    }

    void StaticMesh::calculateSizeBaseOnMesh(const aiMesh* rawMesh, const aiMatrix4x4& transform)
    {
#ifdef DEBUG
        Core::FStopwatch s;
        s.start();
#endif

        glm::vec3 min(std::numeric_limits<float>::max());
        glm::vec3 max(std::numeric_limits<float>::min());

        for (unsigned int i = 0; i < rawMesh->mNumVertices; ++i)
        {
            aiVector3D v = rawMesh->mVertices[i];
            v *= transform;

            min.x = std::min(min.x, v.x);
            min.y = std::min(min.y, v.y);
            min.z = std::min(min.z, v.z);

            max.x = std::max(max.x, v.x);
            max.y = std::max(max.y, v.y);
            max.z = std::max(max.z, v.z);
        }

        _size = Core::FSize3(max - min);
        _center = (max + min) * 0.5f;

#ifdef DEBUG
        debugLog("'{}' processed verticies(size, center) for: {}s; Verticies' count: {}"_f
                 << _name << s.stop() << static_cast<uint64_t>(rawMesh->mNumVertices));
#endif
    }

    StaticMesh StaticMeshFactory::CreateBase(const Core::StringAtom& name /* = ""_atom*/)
    {
        return StaticMesh{ name };
    }

    StaticMesh StaticMeshFactory::CreateBiSide(const Core::StringAtom& name /* = ""_atom*/)
    {
        StaticMesh out{ name };

        out.setDrawModifiers({
            { GL_CULL_FACE, SW::GraphicsComponentData::Modifier::Disable },
        });

        return out;
    }
    StaticMesh StaticMeshFactory::CreateBiBlendSide(const Core::StringAtom& name)
    {
        StaticMesh out{ name };

        out.setDrawModifiers({
            { GL_CULL_FACE, SW::GraphicsComponentData::Modifier::Disable },
            { GL_BLEND, SW::GraphicsComponentData::Modifier::Enable },
        });

        return out;
    }

} // namespace SW