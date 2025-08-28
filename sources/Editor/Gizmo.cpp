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

#include "Gizmo.h"

#include "GameplaySystem/Framework/GameInstance.h"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"

namespace Core
{
    ECS_REGISTER_NEW_COMPONENT_TYPE(Gizmo)

    void Gizmo::initialize()
    {
        _onSelectChangeId = gGameInstance->objectSelectorManager.onChange.subscribeAndGetID(
            [this](BaseComponent* comp, bool newStatus)
            {
                if (comp == this)
                {
                    return;
                }
                if (auto* trans = dynamic_cast<Transformable*>(comp))
                {
                    if ((_isEnabled = newStatus))
                    {
                        setPosition(trans->getPosition());
                    }
                }
            });

        setEnabled(false);
    }

    void Gizmo::load3DModel()
    {
        Assimp::Importer importer;

        const aiScene* scene
            = importer.ReadFile(defaultModelPath.generic_string().c_str(),
                                aiProcess_JoinIdenticalVertices | aiProcess_SortByPType);
        if (Verify(scene) && Verify(scene->mRootNode))
        {
            importFrom(scene->mRootNode, scene, defaultModelPath);
            setShader(gGameInstance->shaderManager.getShaderProgram("color"_atom));
            setOutlineShader(gGameInstance->shaderManager.getShaderProgram("outline"_atom));
        }
    }

    void Gizmo::handleDragStart(StaticMesh* touchedMesh)
    {
        if (gDragDrop.getState() == DragAndDrop::State::Started)
        {
            gDragDrop.payload.type = "gizmo_move"_atom;
            Gizmo::DragData data;
            char directionChar = toupper(touchedMesh->getComponentName()[0]) - 'X';
            data.direction = static_cast<Gizmo::Direction>(directionChar);

            for (auto&& [_, obj] : gGameInstance->objectSelectorManager.getSelectedObjects())
            {
                if (auto* trans = dynamic_cast<Transformable*>(obj.get()))
                {
                    data.attachedObjects.push_back(trans);
                }
            }

            gDragDrop.payload.data = std::make_unique<Gizmo::DragData>(data);
        }
    }

} // namespace Core