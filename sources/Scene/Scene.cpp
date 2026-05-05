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

        static GraphicsComponentData gcd = []()
        {
            GraphicsComponentData data;
            data.generate();
            data.bindVAO();
            data.bindVBO();

            float size = 100.f;
            const std::vector<float> vert = {
                -size, size,  -size, // 0
                -size, -size, -size, // 1
                size,  -size, -size, // 2
                size,  size,  -size, // 3
                -size, size,  size,  // 4
                -size, -size, size,  // 5
                size,  -size, size,  // 6
                size,  size,  size   // 7
            };
            const std::vector<GLuint> ind = { // back face
                                              0, 1, 2, 2, 3, 0,
                                              // left face
                                              1, 5, 4, 4, 0, 1,
                                              // right face
                                              2, 6, 7, 7, 3, 2,
                                              // front face
                                              5, 6, 7, 7, 4, 5,
                                              // top face
                                              4, 7, 3, 3, 0, 4,
                                              // bottom face
                                              1, 2, 6, 6, 5, 1
            };
            data.setVertexBuffer(vert);
            data.setIndexBuffer(ind);

            auto& sm = GetShaderManager();
            // data.setShader(sm.getShaderProgram("2d_main"_atom));

            return data;
        }();

        // gcd.directDraw();

        _postDrawBuffer.resize(0);
        grid.draw();

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