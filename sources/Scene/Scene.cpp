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

#include "Scene.h"

#include "GameplaySystem/Entities/Actor.h"
#include "GameplaySystem/Framework/GameInstance.h"
#include "Graphics/GraphicsComponents.h"
#include "Graphics/ShaderManager.h"

namespace Core
{

    void Scene::directDraw()
    {
        if (gGameInstance->renderMode == GameInstance::RenderMode::Editor)
        {
            gGameInstance->gameEditor.slowObjectPicker.update(*this);
        }

        static BaseGraphicsData gcd = []()
        {
            std::vector<BaseGraphicsData::ModifierParam> modifiers
                = { { .value = BaseGraphicsData::ModifiedValue::CullFace,
                      .modifier = BaseGraphicsData::Modifier::Disable } };

            float w = 100.f;
            float h = 50.f;

            const std::vector<float> vert = {
                0, h, 0, // 0  top-left         | (0 , 64) * (0, 1)
                0, 0, 0, // 1  bottom-left      | (0 , 0 ) * (0, 0)
                w, 0, 0, // 2  bottom-right     | (64, 0 ) * (1, 0)
                w, h, 0  // 3  top-right        | (64, 64) * (1, 1)
            };

            const std::vector<GLuint> ind = {
                0, 1, 2, // triangle 1
                2, 3, 0  // triangle 2
            };

            BaseGraphicsData data;
            data.generate();

            auto* shader = GetShaderManager().getShaderProgram("2d_rect"_atom);
            data.setShader(shader);

            data.setDrawModifiers(std::move(modifiers));

            data.setVertexBuffer(vert);
            data.setIndexBuffer(ind);
            shader->callSetEvent(ShaderProgram::Event::OnSetIndexAndVertexBuffer);

            return data;
        }();

        auto& atlas = GetAssetsManager().getTextureAtlas();
        auto* shader = GetShaderManager().getShaderProgram("2d_rect"_atom);
        shader->use();
        shader->setUniform("uTexture"_atom, 0);
        shader->setUniform("uProjAndView"_atom, gGameInstance->currentCamera->getMatrix());

        atlas.bind();

        Transformable a;

        auto red = atlas.getRect("red.png"_atom);
        auto yellow = atlas.getRect("yellow.png"_atom);

        a.setPosition(GPos3(1.f, 0.f, 0.f));
        a.recalculateMatrices();
        shader->setUniform("uUVOffset"_atom, red.getLeftTop());
        shader->setUniform("uUVSize"_atom, red.getRightBottom() - red.getLeftTop());
        shader->setUniform("uModel"_atom, a.getModelMatrix());
        gcd.directDraw();

        a.setPosition(GPos3(200.f, 0.f, 0.f));
        a.recalculateMatrices();
        shader->setUniform("uUVOffset"_atom, yellow.getLeftTop());
        shader->setUniform("uUVSize"_atom, yellow.getRightBottom() - yellow.getLeftTop());
        shader->setUniform("uModel"_atom, a.getModelMatrix());
        gcd.directDraw();

        a.setPosition(GPos3(500.f, 0.f, 0.f));
        a.recalculateMatrices();
        shader->setUniform("uUVOffset"_atom, glm::vec2(0.f));
        shader->setUniform("uUVSize"_atom, glm::vec2(1.f));
        shader->setUniform("uModel"_atom, a.getModelMatrix());
        gcd.directDraw();

        grid.draw();

        _postDrawBuffer.resize(0);

        for (auto&& asset : _objects)
        {
            auto* object = asset->getData();

            if (!object->isEnabled())
            {
                continue;
            }

            if (auto* mesh = object->tryCastTo<StaticMeshBundle>())
            {
                if (!mesh->isPostDraw())
                {
                    mesh->draw();
                }
                else
                {
                    _postDrawBuffer.push_back(mesh);
                }
            }
        }

        for (auto&& mesh : _postDrawBuffer)
        {
            mesh->draw();
        }

        skybox->draw();
    }

    void Scene::setSceneName(StringAtom name)
    {
        if (Verify(!name.isEmpty()))
        {
            _sceneName = std::move(name);
        }
    }

    /*void Scene::ioFieldsUpdate(DataStream& out)
    {
        auto stream = out.dedicatedNesting("Scene");

        stream.field("sceneName", _sceneName, "Default"_dyn);
    }*/

    const StringAtom& Scene::getSceneName() const noexcept
    {
        return _sceneName;
    }

    void Scene::addToScene(const BaseComponent* object, bool readFromCache)
    {
        if (!Verify(object)) [[unlikely]]
        {
            return;
        }

        _objects.emplace_back(new WorldObject);
        auto* sceneAsset = _objects.back().get();

        if (!sceneAsset->setData(object))
        {
            return;
        }

        auto* added = sceneAsset->getData();

        added->initialize();
        if (readFromCache)
        {
            // added->tryReadFromCache();
        }

        onObjectAdded->trigger(sceneAsset);
    }

    void Scene::addToScene(ECSAsset& asset, bool readFromCache)
    {
        _objects.emplace_back(new WorldObject);
        auto* sceneAsset = _objects.back().get();
        sceneAsset->setAsset(asset);

        if (auto* added = sceneAsset->getData(); Verify(added)) [[likely]]
        {
            added->initialize();

            if (readFromCache)
            {
                // added->tryReadFromCache();
            }

            onObjectAdded->trigger(sceneAsset);
        }
    }

    void Scene::writeToCacheSeparateData()
    {
        for (auto&& asset : _objects)
        {
            // if (auto* object = asset->getData())
            {
                // object->writeToCache();
            }
        }
    }

    std::filesystem::path Scene::getCacheDir() const
    {
        return "scenes";
    }

    StringAtom Scene::getCacheHash() const
    {
        return _sceneName;
    }

    void Scene::initialize()
    {
        skybox = GetAssetsManager().getSkybox("data/assets/baked/skybox/default.nxsky"_atom);
    }

    void Scene::tick(float timeDelta)
    {
        for (auto&& asset : _objects)
        {
            if (auto* object = asset->getData()) [[likely]]
            {
                object->tick(timeDelta);
            }
        }
    }
} // namespace Core
