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

        auto shader = gGameInstance->shaderManager.getShaderProgram("objectIdentifier"_atom);
        if (!Verify(shader)) [[unlikely]]
        {
            return;
        }

        shader->use();
        shader->setUniform("uProjAndView"_atom, camera->getMatrix());
        for (auto&& actor : scene.getActors())
        {
            if (!actor->isEnabled())
            {
                continue;
            }

            auto* bundle = actor->tryCastTo<StaticMeshBundle>();
            if (!bundle || bundle->isExcludedFromSceneDraw())
            {
                continue;
            }
            bundle->tryToRecalculateMatrices();

            for (auto&& mesh : bundle->getRenderTargets())
            {
                if (!mesh->isEnabled())
                {
                    continue;
                }

                shader->setUniform("uModel"_atom, mesh->getModelMatrix());
                Color3 colorId;
                colorId.r = (mesh->getID() & 0x0000FF) >> 0;
                colorId.g = (mesh->getID() & 0x00FF00) >> 8;
                colorId.b = (mesh->getID() & 0xFF0000) >> 16;

                shader->setUniform("uPickingColor"_atom, NormColor3::From(colorId));
                mesh->pureDraw();
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

            unsigned char data[4]{};
            glReadPixels(pickPos.x, pickPos.y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);

            const GLuint pickedID = data[0] + data[1] * 256 + data[2] * 256 * 256;
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

                for (auto&& mesh : bundle->getRenderTargets())
                {
                    if (!mesh->isEnabled())
                    {
                        continue;
                    }

                    if (mesh->getID() == pickedID)
                    {
                        found = mesh;
                        break;
                    }
                }

                if (found)
                {
                    break;
                }
            }

            _callback(found);
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