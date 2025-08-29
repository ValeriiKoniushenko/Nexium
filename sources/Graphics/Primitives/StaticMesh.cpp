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

#include "Core/Timer.h"
#include "GameplaySystem/Framework/GameInstance.h"
#include "Graphics/Image.h"
#include "StaticMeshBundle.h"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"

namespace Core
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
            const auto relative = StringAtom(texturePath.C_Str()).replaceAll("\\", "/");
            const auto resolved
                = (modelPath.parent_path() / relative.toStdString()).lexically_normal();

            Image image;
            if (Verify(image.loadFromFile(resolved, true)))
            {
                setTexture(image.data(), image.getSize().width, image.getSize().height,
                           image.getChannelAsOpenGLType());
            }
        }

        setMesh(rawMesh, true, true);
    }

    nlohmann::json StaticMesh::toJson() const
    {
        nlohmann::json json = BaseComponent::toJson();
        json["Transformable"] = Transformable::toJson();
        json["GraphicsComponentData"] = GraphicsComponentData::toJson();

        return json;
    }

    void StaticMesh::fromJson(const nlohmann::json& json, bool isIgnoreChildren /* = false*/)
    {
        BaseComponent::fromJson(json, isIgnoreChildren);

        if (json.contains("Transformable"))
        {
            Transformable::fromJson(json["Transformable"], isIgnoreChildren);
        }
        if (json.contains("GraphicsComponentData"))
        {
            GraphicsComponentData::fromJson(json["GraphicsComponentData"], isIgnoreChildren);
        }
    }

    void StaticMesh::draw()
    {
        if (!GraphicsComponentData::isValid()) [[unlikely]]
        {
            Assert("Can't draw graphic component. It wasn't configured.");
            return;
        }

        if (!gGameInstance->currentCamera)
        {
            return;
        }

        for (auto [val, mod] : _drawModifiers)
        {
            if (mod.cast() == Modifier::Enable)
            {
                glEnable(val);
            }
            else
            {
                glDisable(val);
            }
        }

        _shader->use();
        _shader->setUniform("uObjectColor"_atom, 1.0f, 1.0f, 1.0f);
        _shader->setUniform("uLightColor"_atom, 1.0f, 1.0f, 1.0f);
        _shader->setUniform("uLightPos"_atom, gGameInstance->world.lightPos);
        _shader->setUniform("uViewPos"_atom, gGameInstance->currentCamera->getPosition());
        _shader->setUniform("uTexture"_atom, 0);
        _shader->setUniform("uProjAndView"_atom, gGameInstance->currentCamera->getMatrix());

        glBindVertexArray(_vao);
        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, _texture);

        applyUniforms();

        if (getIsDrawOutline())
        {
            glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
            glStencilFunc(GL_ALWAYS, 1, 0xFF);
            glStencilMask(0xFF);
            glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(_triangleCount), GL_UNSIGNED_INT,
                           nullptr);
            drawOutline();
        }
        else
        {
            glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(_triangleCount), GL_UNSIGNED_INT,
                           nullptr);
        }

        for (auto [val, mod] : _drawModifiers)
        {
            if (mod.cast() == Modifier::Disable)
            {
                glEnable(val);
            }
            else
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
        FStopwatch s;
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

        _size = FSize3(max - min);
        _center = (max + min) * 0.5f;

#ifdef DEBUG
        debugLog("'{}' processed verticies(size, center) for: {}s; Verticies' count: {}"_f
                 << _name << s.stop() << static_cast<uint64_t>(rawMesh->mNumVertices));
#endif
    }

    void StaticMesh::setOutlineShader(ShaderProgram* sp, bool ignoreVertexAttribSetup)
    {
        _outlineShader = sp;

        if (!ignoreVertexAttribSetup)
        {
            glBindVertexArray(_vao);
            glBindBuffer(GL_ARRAY_BUFFER, _vbo);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
            _outlineShader->setupVertexAttribute();
        }
    }

    void StaticMesh::drawOutline()
    {
        if (!_outlineShader)
        {
            return;
        }

        auto* camera = gGameInstance->currentCamera;

        _outlineShader->use();
        _outlineShader->setUniform("uOutlineColor"_atom,
                                   NormColor4::From(StaticMesh::outlineColor));
        _outlineShader->setUniform("uOutlineSize"_atom, StaticMesh::outlineSize);
        _outlineShader->setUniform("uModel"_atom, _cachedModelMatrix);
        _outlineShader->setUniform("uProjAndView"_atom, camera->getMatrix());

        const float distance = glm::length(camera->getPosition() - glm::vec3(getPosition()));
        const float ndcDistance = distance / camera->getFar();
        _outlineShader->setUniform("uCameraObjectNDCDistance"_atom, ndcDistance);

        glBindVertexArray(_vao);
        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);

        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        glStencilMask(0x00);
        // glDisable(GL_DEPTH_TEST);

        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(_triangleCount), GL_UNSIGNED_INT,
                       nullptr);

        // glEnable(GL_DEPTH_TEST);
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glStencilMask(0xFF);
        glClear(GL_STENCIL_BUFFER_BIT);
        glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    }

    void StaticMesh::pureDraw()
    {
        for (auto [val, mod] : _drawModifiers)
        {
            if (mod.cast() == Modifier::Enable)
            {
                glEnable(val);
            }
            else
            {
                glDisable(val);
            }
        }

        glBindVertexArray(_vao);
        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, _texture);

        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(_triangleCount), GL_UNSIGNED_INT,
                       nullptr);

        for (auto [val, mod] : _drawModifiers)
        {
            if (mod.cast() == Modifier::Disable)
            {
                glEnable(val);
            }
            else
            {
                glDisable(val);
            }
        }
    }

    StaticMeshBundle* StaticMesh::tryToGetRootBundle()
    {
        BaseComponent* i = getParent();
        while (i)
        {
            if (auto* casted = i->tryCastTo<StaticMeshBundle>())
            {
                return casted;
            }

            i = i->getParent();
        }

        return nullptr;
    }

    const StaticMeshBundle* StaticMesh::tryToGetRootBundle() const
    {
        return const_cast<StaticMesh*>(this)->tryToGetRootBundle();
    }

    StaticMesh StaticMeshFactory::CreateBase(const StringAtom& name /* = ""_atom*/)
    {
        return StaticMesh{ name };
    }

    StaticMesh StaticMeshFactory::CreateBiSide(const StringAtom& name /* = ""_atom*/)
    {
        StaticMesh out{ name };

        out.setDrawModifiers({
            { GraphicsComponentData::MV_CullFace, GraphicsComponentData::Modifier::Disable },
        });

        return out;
    }
    StaticMesh StaticMeshFactory::CreateBiBlendSide(const StringAtom& name)
    {
        StaticMesh out{ name };

        out.setDrawModifiers({
            { GraphicsComponentData::MV_CullFace, GraphicsComponentData::Modifier::Disable },
            { GraphicsComponentData::MV_Blend, GraphicsComponentData::Modifier::Enable },
        });

        return out;
    }

} // namespace Core