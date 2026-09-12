// Nexium
// Copyright 2018-2027 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "StaticMesh.h"

#include "Core/Assert.h"
#include "Core/Timer.h"
#include "NxSubsystems/Graphics/Import/AssimpMeshImporter.h"
#include "RawBackend/Image.h"
#include "StaticMeshBundle.h"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"

namespace
{
    // TODO: MOVE & REFACTOR IT!!!
    struct ShaderReflector_default
    {
        struct alignas(16) Light
        {
            alignas(4) float ambientStrength;   // offset 0
            alignas(4) float specularStrength;  // offset 4
            alignas(4) float minLightStrength;  // offset 8
            alignas(4) float specularPow;       // offset 12
            alignas(16) glm::vec3 color;        // offset 16
            alignas(16) glm::vec3 sunDirection; // offset 32
            alignas(16) glm::vec3 viewPos;      // offset 48
        };
        static_assert(sizeof(Light) % 16 == 0);

        inline static Core::StringAtom uTexture = "uTexture"_atom;
        inline static Core::StringAtom uProjAndView = "uProjAndView"_atom;
        inline static Core::StringAtom uModel = "uModel"_atom;
        inline static Core::StringAtom uLight = "Light"_atom;
    };

} // namespace

namespace NX
{
    ECS_IMPL(StaticMesh);

    void StaticMesh::importFrom(const aiMesh* rawMesh, const aiScene* scene,
                                const std::filesystem::path& modelPath /* = ""*/,
                                float scale /* = 1.f*/)
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
            RawBackend::Image image;
            if (const aiTexture* tex = scene->GetEmbeddedTexture(texturePath.C_Str()))
            {
                image.loadFromMemory(reinterpret_cast<const uint8_t*>(tex->pcData), tex->mWidth);
            }
            else
            {
                auto relative = Core::StringAtom(texturePath.C_Str());
                relative.replaceAll("\\", "/");
                const auto resolved
                    = (modelPath.parent_path() / relative.toStdString()).lexically_normal();

                image.loadFromFile(resolved, true);
            }

            if (!image.isEmpty())
            {
                setTexture2D(image.data(), image.getSize().width, image.getSize().height,
                             image.getChannelsCount());
            }
        }

        AssimpMeshImporter::upload(*this, *rawMesh, true, true, scale);
    }

    Core::StringAtom StaticMesh::getCacheHash() const
    {
        return getComponentName();
    }

    void StaticMesh::onDirtyMatrix()
    {
        if (auto* bundle = tryToGetRootBundle())
        {
            bundle->recalculateMatrices();
        }
        else
        {
            recalculateMatrices();
        }
    }

    void StaticMesh::draw(BaseCamera& camera)
    {
        if (!InterleavedGraphicsData::isValid()) [[unlikely]]
        {
            AssertAtCount(false, 2000, "Can't draw graphic component. It wasn't configured.");
            return;
        }

        for (auto [value, mod] : getDrawModifiers())
        {
            if (mod == Modifier::Enable)
            {
                glEnable(static_cast<GLenum>(value));
            }
            else
            {
                glDisable(static_cast<GLenum>(value));
            }
        }

        tryToRecalculateMatrices();

        auto* shader = getShader();
        shader->use();
        bindVAO();
        bindVBO();
        bindEBO();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, getTextureId());
        glBindBuffer(GL_UNIFORM_BUFFER, _uboLight);

        const auto& lightning = GetWorld()->lightning;
        shader->setUniformObject(
            ShaderReflector_default::uLight,
            ShaderReflector_default::Light{ .ambientStrength = lightning.ambientStrength,
                                            .specularStrength = lightning.specularStrength,
                                            .minLightStrength = lightning.minLightStrength,
                                            .specularPow = lightning.specularPow,
                                            .color = lightning.color.toGlm(),
                                            .sunDirection = lightning.sunDirection,
                                            .viewPos = camera.getPosition() });

        shader->setUniform(ShaderReflector_default::uTexture, 0);
        shader->setUniform(ShaderReflector_default::uProjAndView, camera.getMatrix());
        shader->setUniform(ShaderReflector_default::uModel, _cachedModelMatrix);

        if (shouldDrawOutline())
        {
            glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
            glStencilFunc(GL_ALWAYS, 1, 0xFF);
            glStencilMask(0xFF);
            glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(getTriangleCount()), GL_UNSIGNED_INT,
                           nullptr);
            drawOutline(camera);
        }
        else
        {
            glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(getTriangleCount()), GL_UNSIGNED_INT,
                           nullptr);
        }

        for (auto [value, mod] : getDrawModifiers())
        {
            if (mod == Modifier::Disable)
            {
                glEnable(static_cast<GLenum>(value));
            }
            else
            {
                glDisable(static_cast<GLenum>(value));
            }
        }

        for (auto&& comp : _children)
        {
            if (comp->isEnabled())
            {
                if (auto* mesh = comp->tryCastTo<StaticMesh>())
                {
                    mesh->tryToRecalculateMatrices(_cachedModelMatrix);
                    mesh->draw(camera);
                }
                if (auto* bundle = comp->tryCastTo<StaticMeshBundle>())
                {
                    bundle->tryToRecalculateMatrices(_cachedModelMatrix);
                    bundle->draw(camera);
                }
            }
        }
    }

    void StaticMesh::calculateSizeBaseOnMesh(const aiMesh* rawMesh, const aiMatrix4x4& transform)
    {
#ifdef NEXIUM_DEBUG
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
    }

    void StaticMesh::setOutlineShader(ShaderProgram* sp, bool ignoreVertexAttribSetup)
    {
        _outlineShader = sp;

        if (!ignoreVertexAttribSetup)
        {
            bindVAO();
            bindVBO();
            bindEBO();
            _outlineShader->setupVertexAttribute();
        }
    }

    void StaticMesh::drawOutline(BaseCamera& camera)
    {
        if (!_outlineShader)
        {
            return;
        }

        _outlineShader->use();
        _outlineShader->setUniform("uOutlineColor"_atom, Core::NormColor4::From(outlineColor));
        _outlineShader->setUniform("uOutlineSize"_atom, outlineSize);
        _outlineShader->setUniform("uModel"_atom, _cachedModelMatrix);
        _outlineShader->setUniform("uProjAndView"_atom, camera.getMatrix());

        const float distance = glm::length(camera.getPosition() - glm::vec3(getPosition()));
        float ndcDistance = 0.f;
        if (auto* perspectiveCamera = dynamic_cast<PerspectiveCamera*>(&camera))
        {
            ndcDistance = distance / perspectiveCamera->getFar();
        }
        _outlineShader->setUniform("uCameraObjectNDCDistance"_atom, ndcDistance);

        bindVAO();
        bindVBO();
        bindEBO();

        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        glStencilMask(0x00);

        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(getTriangleCount()), GL_UNSIGNED_INT,
                       nullptr);

        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glStencilMask(0xFF);
        glClear(GL_STENCIL_BUFFER_BIT);
        glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    }

    void StaticMesh::recalculateMatrices(const glm::mat4& mat)
    {
        Transformable::recalculateMatrices(mat);
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

    /*void StaticMesh::ioFieldsUpdate(DataStream& out)
    {
        BaseComponent::ioFieldsUpdate(out);
        Transformable::ioFieldsUpdate(out);
        InterleavedGraphicsData::ioFieldsUpdate(out);

        auto stream = out.dedicatedNesting("StaticMesh");

        stream.field("size", _size);
        stream.field("center", _center);
    }*/

    void StaticMesh::pureDraw(const std::function<void(StaticMesh*)>& onUniformSet)
    {
        if (!InterleavedGraphicsData::isValid()) [[unlikely]]
        {
            AssertAtCount(false, 2000, "Can't draw graphic component. It wasn't configured.");
            return;
        }

        for (auto [value, mod] : getDrawModifiers())
        {
            if (mod == Modifier::Enable)
            {
                glEnable(static_cast<GLenum>(value));
            }
            else
            {
                glDisable(static_cast<GLenum>(value));
            }
        }

        bindVAO();
        bindVBO();
        bindEBO();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, getTextureId());

        onUniformSet(this);

        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(getTriangleCount()), GL_UNSIGNED_INT,
                       nullptr);

        for (auto [value, mod] : getDrawModifiers())
        {
            if (mod == Modifier::Disable)
            {
                glEnable(static_cast<GLenum>(value));
            }
            else
            {
                glDisable(static_cast<GLenum>(value));
            }
        }
    }

    void StaticMesh::generate()
    {
        InterleavedGraphicsData::generate();

        glGenBuffers(1, &_uboLight);
        glBindBuffer(GL_UNIFORM_BUFFER, _uboLight);
        glBufferData(GL_UNIFORM_BUFFER, sizeof(ShaderReflector_default::Light), nullptr,
                     GL_STATIC_DRAW);

        glBindBufferBase(GL_UNIFORM_BUFFER, 0, _uboLight);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }

    void StaticMesh::clear()
    {
        InterleavedGraphicsData::clear();
        BaseComponent::clear();

        glDeleteBuffers(1, &_uboLight);
        _uboLight = 0;
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

    StaticMesh StaticMeshFactory::CreateBase(const Core::StringAtom& name /* = ""_atom*/)
    {
        return StaticMesh{ name };
    }

    StaticMesh StaticMeshFactory::CreateBiSide(const Core::StringAtom& name /* = ""_atom*/)
    {
        StaticMesh out{ name };

        out.setDrawModifiers({
            { .value = BaseGraphicsData::ModifiedValue::CullFace,
              .modifier = BaseGraphicsData::Modifier::Disable },
        });

        return out;
    }

    StaticMesh StaticMeshFactory::CreateBiBlendSide(const Core::StringAtom& name)
    {
        StaticMesh out{ name };

        out.setDrawModifiers({
            { .value = BaseGraphicsData::ModifiedValue::CullFace,
              .modifier = BaseGraphicsData::Modifier::Disable },
            { .value = BaseGraphicsData::ModifiedValue::Blend,
              .modifier = BaseGraphicsData::Modifier::Enable },
        });

        return out;
    }
} // namespace NX
