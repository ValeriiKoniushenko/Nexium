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

#include "GameplaySystem/Framework/GameInstance.h"
#include "Graphics/GraphicsComponents.h"
#include "Graphics/ShaderManager.h"

namespace Core
{
    Scene::Scene() = default;

    void Scene::directDraw()
    {
        if (gGameInstance->renderMode == GameInstance::RenderMode::Editor)
        {
            gGameInstance->gameEditor.slowObjectPicker.update(*this);
        }

        static InterleavedGraphicsData gcd = []()
        {
            std::vector<InterleavedGraphicsData::ModifierParam> modifiers
                = { { .value = InterleavedGraphicsData::ModifiedValue::CullFace,
                      .modifier = InterleavedGraphicsData::Modifier::Disable } };

            float w = 200.f;
            float h = 100.f;

            const std::vector<float> vert = {
                -w, h,  0, // 0  top-left
                -w, -h, 0, // 1  bottom-left
                w,  -h, 0, // 2  bottom-right
                w,  h,  0  // 3  top-right
            };
            const std::vector<float> uvs = {
                0.f, 1.f, // 0  top-left
                0.f, 0.f, // 1  bottom-left
                1.f, 0.f, // 2  bottom-right
                1.f, 1.f  // 3  top-right
            };

            const std::vector<GLuint> ind = {
                0, 1, 2, // triangle 1
                2, 3, 0  // triangle 2
            };

            InterleavedGraphicsData data;
            data.generate();
            data.setShader(GetShaderManager().getShaderProgram("2d_main"_atom));
            data.bindVAO();
            data.bindVBO();

            data.setDrawModifiers(std::move(modifiers));
            data.setVertexBuffer(vert);
            data.setIndexBuffer(ind);

            data.unbindVao();

            return data;
        }();

        auto* shader = GetShaderManager().getShaderProgram("2d_main"_atom);
        shader->use();
        shader->setUniform("uTexture"_atom, 0);
        shader->setUniform("uProjAndView"_atom, gGameInstance->currentCamera->getMatrix());
        shader->setUniform("uModel"_atom, glm::mat4(1.f));

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