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

#include "StaticMesh.h"

#include "Core/Timer.h"
#include "GameplaySystem/Framework/GameInstance.h"
#include "Graphics/Image.h"
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

namespace Core
{
    ECS_COMPONENT_IMPL(StaticMesh)

    void StaticMesh::importFrom(const aiMesh* rawMesh, const aiScene* scene,
                                const std::filesystem::path& modelPath /* = ""*/,
                                float scale /* = 1.f*/)
    {
        if (!DEBUG_ASSERT_VAL(rawMesh)) [[unlikely]]
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
            Image image;
            if (const aiTexture* tex = scene->GetEmbeddedTexture(texturePath.C_Str()))
            {
                image.loadFromMemory(reinterpret_cast<const uint8_t*>(tex->pcData), tex->mWidth);
            }
            else
            {
                auto relative = StringAtom(texturePath.C_Str());
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

        setMesh(rawMesh, true, true, scale);
    }

    StringAtom StaticMesh::getCacheHash() const
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

    void StaticMesh::draw()
    {
        if (!GraphicsComponentData::isValid()) [[unlikely]]
        {
            DEBUG_ASSERT("Can't draw graphic component. It wasn't configured.");
            return;
        }

        if (!gGameInstance->currentCamera) [[unlikely]]
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

        tryToRecalculateMatrices();

        _shader->use();
        glBindVertexArray(_vao);
        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, _texture);
        glBindBuffer(GL_UNIFORM_BUFFER, _uboLight);

        const auto& lightning = GetWorld().lightning;
        _shader->setUniformObject(
            ShaderReflector_default::uLight,
            ShaderReflector_default::Light{ lightning.ambientStrength, lightning.specularStrength,
                                            lightning.minLightStrength, lightning.specularPow,
                                            lightning.color.toGlm(), lightning.sunDirection,
                                            gGameInstance->currentCamera->getPosition() });

        _shader->setUniform(ShaderReflector_default::uTexture, 0);
        _shader->setUniform(ShaderReflector_default::uProjAndView,
                            gGameInstance->currentCamera->getMatrix());
        _shader->setUniform(ShaderReflector_default::uModel, _cachedModelMatrix);

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
            if (comp->isEnabled())
            {
                if (auto* mesh = comp->tryCastTo<StaticMesh>())
                {
                    mesh->tryToRecalculateMatrices(_cachedModelMatrix);
                    mesh->draw();
                }
                if (auto* bundle = comp->tryCastTo<StaticMeshBundle>())
                {
                    bundle->tryToRecalculateMatrices(_cachedModelMatrix);
                    bundle->draw();
                }
            }
        }
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
        _outlineShader->setUniform("uOutlineColor"_atom, NormColor4::From(outlineColor));
        _outlineShader->setUniform("uOutlineSize"_atom, outlineSize);
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

        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(_triangleCount), GL_UNSIGNED_INT,
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

    void StaticMesh::ioFieldsUpdate(DataStream& stream)
    {
        BaseComponent::ioFieldsUpdate(stream);
        Transformable::ioFieldsUpdate(stream);
        GraphicsComponentData::ioFieldsUpdate(stream);

        stream.field("size", _size);
        stream.field("center", _center);
    }

    void StaticMesh::pureDraw(const std::function<void(StaticMesh*)>& onUniformSet)
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

        onUniformSet(this);

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

    void StaticMesh::generate()
    {
        GraphicsComponentData::generate();
        glGenBuffers(1, &_uboLight);
        glBindBuffer(GL_UNIFORM_BUFFER, _uboLight);
        glBufferData(GL_UNIFORM_BUFFER, sizeof(ShaderReflector_default::Light), nullptr,
                     GL_STATIC_DRAW);

        glBindBufferBase(GL_UNIFORM_BUFFER, 0, _uboLight);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }

    void StaticMesh::clear()
    {
        GraphicsComponentData::clear();
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