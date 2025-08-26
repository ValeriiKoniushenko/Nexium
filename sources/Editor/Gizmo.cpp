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

    void Gizmo::initialize()
    {
        Assimp::Importer importer;

        const aiScene* scene
            = importer.ReadFile(defaultModelPath.generic_string().c_str(),
                                aiProcess_JoinIdenticalVertices | aiProcess_SortByPType);
        if (Verify(scene) && Verify(scene->mRootNode))
        {
            _mesh.importFrom(scene->mRootNode, scene, defaultModelPath);
            _mesh.setShader(gGameInstance->shaderManager.getShaderProgram("color"_atom));
            _mesh.setOutlineShader(gGameInstance->shaderManager.getShaderProgram("outline"_atom));
        }

        _onSelectChangeId = gGameInstance->objectSelectorManager.onChange.subscribeAndGetID(
            [this](BaseComponent* comp, bool newStatus)
            {
                if (auto* trans = dynamic_cast<Transformable*>(comp))
                {
                    if ((_isEnabled = newStatus))
                    {
                        _mesh.setPosition(trans->getPosition());
                    }
                }
            });
    }

    void Gizmo::drawAndUpdate()
    {
        _mesh.draw();
    }

} // namespace Core