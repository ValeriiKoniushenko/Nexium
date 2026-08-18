/*
 * MIT License
 *
 * Copyright (c) 2018-2027 Valerii Koniushenko
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

#include "ObjectPicker.h"

#include "Editor/Windows/GameViewport.h"
#include "GameplaySystem/Framework/GameInstance.h"
#include "Graphics/Primitives/StaticMeshBundle.h"
#include "Scene/Rectangle.h"

namespace Core
{
    void BaseObjectPicker::update(Scene& scene)
    {
        const auto* world = GetWorld();
        if (!_requested || !world || !world->currentCamera)
        {
            _requested = false;
            return;
        }
        _requested = false;

        glm::vec2 pickPos = {};
        auto* gameViewportWnd = gGameInstance->gameEditor.getWindow<GameViewportEWC>();
        if (!gameViewportWnd)
        {
            return;
        }
        pickPos = getPickedObject(gameViewportWnd);

        onRequest(scene, world->currentCamera, pickPos);
    }

    glm::vec2 BaseObjectPicker::getPickedObject(const GameViewportEWC* wnd)
    {
        if (!wnd) [[unlikely]]
        {
            return {};
        }

        const auto wndPos = wnd->getInnerPosition();
        auto pickPos = Mouse::GetPosition();
        pickPos.x -= wndPos.x;
        pickPos.y -= wndPos.y;
        pickPos.y = wnd->getInnerWindowSize().height - pickPos.y - 1;

        return pickPos;
    }

    void BaseObjectPicker::requestPick(const std::function<void(Transformable*)>& callback)
    {
        _callback = callback;
        _requested = true;
    }

    void SlowObjectPicker::onRequest(Scene& scene, BaseCamera* camera, glm::vec2 pickPos)
    {
        const auto pickedColor = drawingPreparations(scene, camera, pickPos);
        if (!pickedColor)
        {
            return;
        }

        pickingUpTheObjectBasedOnColor(scene, *pickedColor);
    }

    std::optional<Color3> SlowObjectPicker::drawingPreparations(Scene& scene, BaseCamera* camera,
                                                                glm::vec2 pickPos)
    {
        auto* shader = gGameInstance->shaderManager.getShaderProgram("objectIdentifier"_atom);
        if (!Verify(shader)) [[unlikely]]
        {
            return std::nullopt;
        }

        auto onUniformSet = [&shader](StaticMesh* mesh)
        {
            shader->setUniform("uModel"_atom, mesh->getModelMatrix());
            shader->setUniform("uPickingColor"_atom, NormColor3::From(mesh->toUniqueColor()));
        };
        static auto preRenderCond = [](const Actor* actor) { return !actor->isPostDraw(); };
        static auto postRenderCond = [](const Actor* actor) { return actor->isPostDraw(); };

        // Drawing with needed shader to highlight possible for pick up objects.
        // The idea is to draw all objects in different colors. And the picked up object
        // will literally means that the mouse cursor is overlapping some color(and the color ->
        // it's a real object). E.g.: red -> Tree; blue -> House; purple -> Car.
        shader->use();
        shader->setUniform("uProjAndView"_atom, camera->getMatrix());

        // First shot with preRenderCond
        for (auto&& object : scene.getObjects())
        {
            if (!object->isEnabled())
            {
                continue;
            }

            if (auto* bundle = object->tryCastTo<StaticMeshBundle>())
            {
                bundle->pureDraw(onUniformSet, preRenderCond);
            }
        }

        // Second shot with postRenderCond
        for (auto&& object : scene.getObjects())
        {
            if (!object->isEnabled())
            {
                continue;
            }

            if (auto* bundle = object->tryCastTo<StaticMeshBundle>())
            {
                bundle->pureDraw(onUniformSet, postRenderCond);
            }
        }

        glFlush();
        glFinish();

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        std::array<unsigned char, 4> pickedData = { 0 };
        glReadPixels(static_cast<GLint>(pickPos.x), static_cast<GLint>(pickPos.y), 1, 1, GL_RGBA,
                     GL_UNSIGNED_BYTE, pickedData.data());

        return Color3{ pickedData[0], pickedData[1], pickedData[2] };
    }

    void SlowObjectPicker::pickingUpTheObjectBasedOnColor(Scene& scene, Color3 pickedColor)
    {
        // The image with all objects are generated above.
        // Now, let's calculate the logic.
        const StaticMesh* found = nullptr;
        for (auto&& object : scene.getObjects())
        {
            if (!object->isEnabled())
            {
                continue;
            }

            auto* bundle = object->tryCastTo<StaticMeshBundle>();
            if (!bundle)
            {
                continue;
            }

            // Checking the StaticMesh components of the StaticMeshBundle
            bundle->forEach(
                [pickedColor, &found](BaseComponent* component)
                {
                    Assert(component);
                    if (component && component->isEnabled() && component->isTypeOf<StaticMesh>())
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

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void RectangleBasedObjectPicker::onRequest(Scene& scene, BaseCamera* camera, glm::vec2 pickPos)
    {
        return; // Not impl

        const SceneObj::Rectangle* pickedRect = nullptr;

        for (auto& object : scene.getObjects())
        {
            if (!object->isEnabled())
            {
                continue;
            }

            auto* rectangle = object->tryCastTo<SceneObj::Rectangle>();
            if (!rectangle)
            {
                continue;
            }

            auto rectPos = rectangle->getPosition();
            auto rectSize = rectangle->getDrawRectSize();
        }

        if (_callback && pickedRect)
        {
            _callback(pickedRect);
        }
    }

    ObjectPickerAggregator::ObjectPickerAggregator()
    {
        _pickers.emplace_back(std::make_unique<SlowObjectPicker>());
        _pickers.emplace_back(std::make_unique<RectangleBasedObjectPicker>());
    }

    void ObjectPickerAggregator::update(Scene& scene)
    {
        for (const auto& picker : _pickers)
        {
            picker->update(scene);
        }
    }

    void ObjectPickerAggregator::requestPick(const std::function<void(Transformable*)>& callback)
    {
        _callback = callback;
        _requested = true;
        for (const auto& picker : _pickers)
        {
            picker->requestPick(
                [this](Transformable* object)
                {
                    if (_requested)
                    {
                        _callback(object);
                        _requested = false;
                    }
                });
        }
    }
} // namespace Core