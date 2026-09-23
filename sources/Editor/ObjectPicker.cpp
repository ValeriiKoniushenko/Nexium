// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "ObjectPicker.h"

#include "Editor/EditorIntegration.h"
#include "Editor/Windows/GameViewport.h"
#include "NxWorld/Entities/Camera/Camera.h"
#ifdef NEXIUM_ENABLE_3D_MODULE
    #include "NxWorld/Entities/Mesh/StaticMeshBundle.h"
#endif
#include "NxWorld/Framework/GameInstance.h"
#include "NxWorld/Scene/Rectangle.h"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/string_cast.hpp"

using namespace NX;
using namespace Platform;

namespace NX
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
        auto* gameViewportWnd = GetEditor()->getWindow<GameViewportEWC>();
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

#ifdef NEXIUM_ENABLE_3D_MODULE
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
        StaticMesh* found = nullptr;
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

        if (_callback && found)
        {
            _callback(found);
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
#endif

    void RectangleBasedObjectPicker::onRequest(Scene& scene, BaseCamera* camera, glm::vec2 pickPos)
    {
        const auto frameSize = camera->getOutputFrameSize();
        if (frameSize.width <= 0.f || frameSize.height <= 0.f)
        {
            return;
        }
        const glm::vec2 ndc{
            2.f * pickPos.x / frameSize.width - 1.f,
            2.f * pickPos.y / frameSize.height - 1.f,
        };

        SceneObj::Rectangle* pickedRect = nullptr;

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
            const glm::mat4& model = rectangle->getModelMatrix();
            const float modelDeterminant = glm::determinant(model);
            if (!std::isfinite(modelDeterminant) || modelDeterminant == 0.f)
            {
                continue;
            }

            const glm::mat4 inverseMvp = glm::inverse(camera->getMatrix() * model);
            glm::vec4 nearPoint = inverseMvp * glm::vec4(ndc, -1.f, 1.f);
            glm::vec4 farPoint = inverseMvp * glm::vec4(ndc, 1.f, 1.f);
            nearPoint /= nearPoint.w;
            farPoint /= farPoint.w;
            const glm::vec3 rayOrigin = nearPoint;
            const glm::vec3 rayDirection = glm::vec3(farPoint - nearPoint);

            if (std::abs(rayDirection.z) <= std::numeric_limits<float>::epsilon())
            {
                continue;
            }
            // Intersection with the rectangle's local XY plane.
            const float t = -rayOrigin.z / rayDirection.z;
            if (t < 0.f || t > 1.f)
            {
                continue;
            }
            const glm::vec3 localHit = rayOrigin + rayDirection * t;
            constexpr float size = SceneObj::Rectangle::GetDefaultDrawRectSize();

            auto rectPos = rectangle->getPosition();
            auto rectSize = rectangle->getDrawRectSize();
            rectPos -= camera->getGlobalPosition();

            if (localHit.x >= 0.f && localHit.x <= size && localHit.y >= 0.f && localHit.y <= size)
            {
                pickedRect = rectangle;
                break;
            }
        }

        if (_callback && pickedRect)
        {
            std::invoke(_callback, pickedRect);
        }
    }

    ObjectPickerAggregator::ObjectPickerAggregator()
    {
#ifdef NEXIUM_ENABLE_3D_MODULE
        _pickers.emplace_back(std::make_unique<SlowObjectPicker>());
#endif
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
} // namespace NX
