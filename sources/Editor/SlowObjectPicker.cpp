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

#include "SlowObjectPicker.h"

#include "Editor/Windows/GameViewport.h"
#include "GameplaySystem/Framework/GameInstance.h"

namespace Core
{
    void SlowObjectPicker::update(Scene& scene)
    {
        if (!_requested)
        {
            return;
        }

        auto* camera = gGameInstance->currentCamera;
        if (!camera)
        {
            return;
        }

        auto* shader = gGameInstance->shaderManager.getShaderProgram("objectIdentifier"_atom);
        if (!DEBUG_ASSERT_VAL(shader)) [[unlikely]]
        {
            return;
        }

        static auto onUniformSet = [&shader](StaticMesh* mesh)
        {
            shader->setUniform("uModel"_atom, mesh->getModelMatrix());
            shader->setUniform("uPickingColor"_atom, NormColor3::From(mesh->toUniqueColor()));
        };

        static auto preRenderCond = [](const Actor* actor)
        {
            return !actor->isPostDraw();
        };

        static auto postRenderCond = [](const Actor* actor)
        {
            return actor->isPostDraw();
        };

        shader->use();
        shader->setUniform("uProjAndView"_atom, camera->getMatrix());
        for (auto&& actor : scene.getActors())
        {
            if (!actor->isEnabled())
            {
                continue;
            }

            if (auto* bundle = actor->tryCastTo<StaticMeshBundle>())
            {
                bundle->pureDraw(onUniformSet, preRenderCond);
            }
        }

        for (auto&& actor : scene.getActors())
        {
            if (!actor->isEnabled())
            {
                continue;
            }

            if (auto* bundle = actor->tryCastTo<StaticMeshBundle>())
            {
                bundle->pureDraw(onUniformSet, postRenderCond);
            }
        }

        glFlush();
        glFinish();

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        if (auto* wnd = gGameInstance->gameEditor.getWindow<GameViewportEWC>())
        {
            const auto wndPos = wnd->getInnerPosition();
            auto pickPos = Mouse::GetPosition();
            pickPos.x -= wndPos.x;
            pickPos.y -= wndPos.y;
            pickPos.y = wnd->getInnerWindowSize().height - pickPos.y - 1;

            unsigned char pickedData[4] = { 0 };
            glReadPixels(static_cast<GLint>(pickPos.x), static_cast<GLint>(pickPos.y), 1, 1,
                         GL_RGBA, GL_UNSIGNED_BYTE, pickedData);

            Color3 pickedColor{ pickedData[0], pickedData[1], pickedData[2] };

            StaticMesh* found = nullptr;
            for (auto&& actor : scene.getActors())
            {
                if (!actor->isEnabled())
                {
                    continue;
                }

                auto* bundle = actor->tryCastTo<StaticMeshBundle>();
                if (!bundle)
                {
                    continue;
                }

                bundle->forEach(
                    [pickedColor, &found](BaseComponent* component)
                    {
                        DEBUG_ASSERT(component);
                        if (component && component->isEnabled()
                            && component->isTypeOf<StaticMesh>())
                        {
                            auto* mesh = component->castTo<StaticMesh>();
                            if (mesh->isMatchUniqueColor(pickedColor))
                            {
                                found = mesh;
                                return false;
                            }
                        }
                        return true;
                    });

                if (found)
                {
                    break;
                }
            }

            if (_callback)
            {
                _callback(found);
            }
        }
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        _requested = false;
    }

    void SlowObjectPicker::requestPick(std::function<void(StaticMesh*)>&& callback)
    {
        _callback = std::forward<decltype(callback)>(callback);
        _requested = true;
    }
} // namespace Core