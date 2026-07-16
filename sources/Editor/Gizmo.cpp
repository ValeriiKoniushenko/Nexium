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

#include "Gizmo.h"

#include "GameplaySystem/Framework/GameInstance.h"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"

namespace Core
{
    ECS_COMPONENT_IMPL(Gizmo);

    void Gizmo::draw(BaseCamera& camera)
    {
        glDepthFunc(GL_ALWAYS);
        StaticMeshBundle::draw(camera);
        glDepthFunc(GL_LESS);
    }

    void Gizmo::pureDraw(const std::function<void(StaticMesh*)>& onUniformSet,
                         const std::function<bool(const Actor*)>& conditional)
    {
        glDepthFunc(GL_ALWAYS);
        StaticMeshBundle::pureDraw(onUniformSet, conditional);
        glDepthFunc(GL_LESS);
    }

    void Gizmo::onMousePicked(StaticMesh* clickedPart)
    {
        StaticMeshBundle::onMousePicked(clickedPart);

        handleDragStart(clickedPart);
    }

    void Gizmo::onTick(float delta)
    {
        StaticMeshBundle::onTick(delta);

        handleDrag();
    }

    void Gizmo::initialize()
    {
        StaticMeshBundle::initialize();

        load3DModel();
        setIsPostDraw(true);
        setIsIgnoreSelect(true);
    }

    void Gizmo::load3DModel()
    {
        const auto defaultModelPath = Config::Path::objects3d / "Models/FBX/gizmo.fbx";

        static Assimp::Importer importer;
        static const aiScene* scene
            = importer.ReadFile(defaultModelPath.string().c_str(),
                                aiProcess_JoinIdenticalVertices | aiProcess_SortByPType);
        if (Verify(scene) && Verify(scene->mRootNode))
        {
            importFrom(scene->mRootNode, scene, defaultModelPath);
            setShader(gGameInstance->shaderManager.getShaderProgram("pickUpColorFiller"_atom));
        }
    }

    void Gizmo::handleDragStart(StaticMesh* touchedMesh)
    {
        if (gDragDrop.getState() == DragAndDrop::State::Started)
        {
            gDragDrop.payload.type = DragData::dragType;
            DragData data;
            const char directionChar
                = static_cast<char>(toupper(touchedMesh->getComponentName()[0])) - 'X';

            int* ptr = NULL;

            data.direction = static_cast<Direction>(directionChar);

            for (auto& obj :
                 gGameInstance->objectSelectorManager.getSelectedObjects() | std::views::values)
            {
                if (auto* trans = dynamic_cast<Transformable*>(obj.get()))
                {
                    data.attachedObjects.push_back(trans);
                }
            }

            gDragDrop.payload.data = std::make_unique<DragData>(data);

            _lastRay.reset();
        }
    }

    void Gizmo::handleDrag()
    {
        if (!gDragDrop.isTypeOf<DragData>())
        {
            return;
        }

        const auto* data = dynamic_cast<DragData*>(gDragDrop.payload.data.get());
        if (!Verify(data))
        {
            return;
        }

        if (!gGameInstance->currentCamera)
        {
            return;
        }

        auto* camera = gGameInstance->currentCamera;
        const float distance = glm::length(camera->getPosition() - glm::vec3(getPosition()));
        auto ray = camera->putMouseRay(distance);

        if (!_lastRay)
        {
            _lastRay = ray;
        }

        const auto delta = ray - *_lastRay;

        if (data->direction == Direction::X)
        {
            for (const auto& obj : data->attachedObjects)
            {
                obj->moveRight(delta.x);
            }
        }
        if (data->direction == Direction::Y)
        {
            for (const auto& obj : data->attachedObjects)
            {
                obj->moveUp(-delta.y);
            }
        }
        if (data->direction == Direction::Z)
        {
            for (const auto& obj : data->attachedObjects)
            {
                obj->moveForward(delta.z);
            }
        }

        _lastRay = ray;
    }

    void Gizmo::recalculateMatrices(const glm::mat4& mat)
    {
        // We should recalc only transform

        _cachedModelMatrix = glm::mat4(1.f);
        _cachedModelMatrix[3][0] = mat[3][0];
        _cachedModelMatrix[3][1] = mat[3][1];
        _cachedModelMatrix[3][2] = mat[3][2];
        _cachedModelMatrix[3][3] = mat[3][3];
        _cachedModelMatrix = glm::translate(_cachedModelMatrix, _position);
        StaticMeshBundle::recalculateMatrices(_cachedModelMatrix);

        _isDirtyModelMatrix = false;
    }

} // namespace Core
