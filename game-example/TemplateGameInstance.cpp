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

#include "TemplateGameInstance.h"

#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"

std::unique_ptr<TemplateGameInstance> gameInstance = std::make_unique<TemplateGameInstance>();

void TemplateGameInstance::onLoadShaders()
{
    auto* shader = shaderManager->getShaderProgram("color"_atom);
    if (Verify(shader))
    {
        shader->setVertexAttributeCallback(
            []()
            {
                glEnableVertexAttribArray(0);
                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), nullptr);

                glEnableVertexAttribArray(1);
                glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                                      reinterpret_cast<void*>(3 * sizeof(float)));

                glEnableVertexAttribArray(2);
                glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                                      reinterpret_cast<void*>(6 * sizeof(float)));
            });
    }
}

void TemplateGameInstance::onTick()
{
    keyboardInput.update();
    mouseInput.update();

    auto* shader = shaderManager->getShaderProgram("color"_atom);
    shader->use();
    shader->setUniform("uObjectColor"_atom, 1.0f, 1.0f, 1.0f);
    shader->setUniform("uLightColor"_atom, 1.0f, 1.0f, 1.0f);
    shader->setUniform("uLightPos"_atom, world.lightPos);
    shader->setUniform("uViewPos"_atom, camera.getPosition());
    shader->setUniform("uTexture"_atom, 0);
    shader->setUniform("uProjAndView"_atom, camera.getMatrix());

    meshes.front().directDraw();
}

void TemplateGameInstance::onInitFinish()
{
    camera.moveForward(-100);
    camera.setFov(90.f);

    // clang-format off
    constexpr float speed = 50.f, mouseSensitivity = 700.0;
    auto getRealSpeed = [speed](SW::KeyboardIA::SpecKeysState state)
    {
        const float mlt = state.leftShift.cast() == SW::Keyboard::KeyState::Pressed ? 10.f : 1.f;
        return speed * mlt;
    };
    keyboardInput.create("moveForward", GLFW_KEY_W)->onPress.subscribe([&](auto state){ camera.moveForward(getRealSpeed(state) * world.timeDelta); });
    keyboardInput.create("moveBackward", GLFW_KEY_S)->onPress.subscribe([&](auto state){ camera.moveForward(-getRealSpeed(state) * world.timeDelta); });
    keyboardInput.create("moveRight", GLFW_KEY_D)->onPress.subscribe([&](auto state){ camera.moveRight(-getRealSpeed(state) * world.timeDelta); });
    keyboardInput.create("moveLeft", GLFW_KEY_A)->onPress.subscribe([&](auto state){ camera.moveRight(getRealSpeed(state) *   world.timeDelta); });
    keyboardInput.create("moveUp", GLFW_KEY_SPACE)->onPress.subscribe([&](auto state){ camera.moveUp(-getRealSpeed(state) *  world.timeDelta); });
    keyboardInput.create("moveDown", GLFW_KEY_C)->onPress.subscribe([&](auto state){ camera.moveUp(getRealSpeed(state) *     world.timeDelta); });
    keyboardInput.create("exit", GLFW_KEY_ESCAPE)->onPress.subscribe([&](auto){ window->close(); });
    const auto toggleCursorMode = keyboardInput.create("toggleCursorMode", GLFW_KEY_M);
    toggleCursorMode->onPress.subscribe([&](auto) { window->toggleCursorMode(); });
    toggleCursorMode->setIsRepeatable(false);
    mouseInput.create("cameraView", 0)->onMove.subscribe([&](glm::vec2 delta, auto){ camera.yawAndPitch(delta * world.timeDelta * mouseSensitivity); });
    // clang-format on

    std::vector<std::filesystem::path> modelPaths
        = { /*"assets/base-3d/Models/FBX/Tree.fbx",*/ "assets/base-3d/Models/FBX/FireHydrant.fbx" };

    Assimp::Importer importer;
    for (auto&& path : modelPaths)
    {
        const aiScene* scene = importer.ReadFile(
            path.generic_string().c_str(),
            aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType);
        if (Verify(scene) && Verify(scene->mRootNode))
        {
            SW::StaticMeshBundle mesh;
            mesh.importFrom(scene->mRootNode, scene, path);
            mesh.setShaderProgram(shaderManager->getShaderProgram("color"_atom));
            meshes.push_back(std::move(mesh));
        }
    }
}
