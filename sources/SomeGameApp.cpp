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

#include "Camera/Camera.h"
#include "Core/Timer.h"
#include "Editor/Server.h"
#include "Graphics/Image.h"
#include "Graphics/Primitives/StaticMesh.h"
#include "Graphics/ShaderManager.h"
#include "Graphics/Window.h"
#include "InputDevices/InputManager.h"
#include "Misc/FPSCounter.h"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/string_cast.hpp"

#include <iostream>

using namespace SW;

int main()
{
#ifdef DEBUG
    spdlog::set_level(spdlog::level::trace);
#endif
    std::cout << std::fixed << std::setprecision(15);
    spdlog::set_pattern("%D [%L] [%n] %v");

    //    _____
    //   /  ___|
    //   \ `--.   ___  _ __ __   __ ___  _ __
    //    `--. \ / _ \| '__|\ \ / // _ \| '__|
    //   /\__/ /|  __/| |    \ V /|  __/| |
    //   \____/  \___||_|     \_/  \___||_|
    //------------------------------------------
    auto& server = GetEditorServer();
    server.setPort(61005);
    server.initialize();
    server.start();

    //    _    _  _             _
    //   | |  | |(_)           | |
    //   | |  | | _  _ __    __| |  ___ __      __
    //   | |/\| || || '_ \  / _` | / _ \\ \ /\ / /
    //   \  /\  /| || | | || (_| || (_) |\ V  V /
    //    \/  \/ |_||_| |_| \__,_| \___/  \_/\_/
    //----------------------------------------------
    auto& window = GetWindow();
    window.create("Sprite Walker", Core::ISize2{ 1200, 800 });
    // window.toggleCursorMode();

    //    _____  _                 _
    //   /  ___|| |               | |
    //   \ `--. | |__    __ _   __| |  ___  _ __  ___
    //    `--. \| '_ \  / _` | / _` | / _ \| '__|/ __|
    //   /\__/ /| | | || (_| || (_| ||  __/| |   \__ \
    //   \____/ |_| |_| \__,_| \__,_| \___||_|   |___/
    //-------------------------------------------------
    auto& sm = ShaderManager::instance();
    sm.loadShaders("assets/shaders");
    sm.debugLog("Was loaded {} shaders."_f << sm.countOfShaders());
    for (const auto& notLoadedShader : sm.getFailedShaders())
    {
        sm.warnLog("Shader '{}' found but not loaded. It contains some error[s]."_f
                   << notLoadedShader);
    }

    // ======== Setting up[s] ==========
    auto* shader = sm.getShaderProgram("color"_atom);
    Assert(shader);
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

    //    _    _               _      _     _   _
    //   | |  | |             | |    | |   | | | |
    //   | |  | |  ___   _ __ | |  __| |   | | | |  __ _  _ __  ___
    //   | |/\| | / _ \ | '__|| | / _` |   | | | | / _` || '__|/ __|
    //   \  /\  /| (_) || |   | || (_| |   \ \_/ /| (_| || |   \__ \
    //    \/  \/  \___/ |_|   |_| \__,_|    \___/  \__,_||_|   |___/
    //---------------------------------------------------------------
    float timeDelta = 0.f;
    glm::vec3 lightPos(1'000'000.f, 1'000'000.f, 1'000'000.f);
    BaseCamera camera;
    camera.moveForward(-100);
    camera.setFov(90.f);
    std::vector<StaticMesh> meshes;

    //    _____                       _        ___         _    _
    //   |_   _|                     | |      / _ \       | |  (_)
    //     | |   _ __   _ __   _   _ | |_    / /_\ \  ___ | |_  _   ___   _ __   ___
    //     | |  | '_ \ | '_ \ | | | || __|   |  _  | / __|| __|| | / _ \ | '_ \ / __|
    //    _| |_ | | | || |_) || |_| || |_    | | | || (__ | |_ | || (_) || | | |\__ \
    //    \___/ |_| |_|| .__/  \__,_| \__|   \_| |_/ \___| \__||_| \___/ |_| |_||___/
    //                 | |
    //                 |_|
    //--------------------------------------------------------------------------------
    KeyboardInputManger keyboardInput;
    MouseInputManger mouseInput;

    // clang-format off
    constexpr float speed = 10.f, mouseSensitivity = 700.0;
    auto getRealSpeed = [speed](KeyboardIA::SpecKeysState state)
    {
        const float mlt = state.leftShift.cast() == Keyboard::KeyState::Pressed ? 15.f : 1.f;
        return speed * mlt;
    };
    auto lookAtRandom = keyboardInput.create("lookAtObject", GLFW_KEY_L);
    lookAtRandom->onPress.subscribe([&](auto){ camera.lookAt(meshes.at(rand() % meshes.size()).getCenter()); });
    lookAtRandom->setIsRepeatable(false);
    keyboardInput.create("moveForward", GLFW_KEY_W)->onPress.subscribe([&](auto state){ camera.moveForward(getRealSpeed(state) * timeDelta); });
    keyboardInput.create("moveBackward", GLFW_KEY_S)->onPress.subscribe([&](auto state){ camera.moveForward(-getRealSpeed(state) * timeDelta); });
    keyboardInput.create("moveRight", GLFW_KEY_D)->onPress.subscribe([&](auto state){ camera.moveRight(-getRealSpeed(state) * timeDelta); });
    keyboardInput.create("moveLeft", GLFW_KEY_A)->onPress.subscribe([&](auto state){ camera.moveRight(getRealSpeed(state) * timeDelta); });
    keyboardInput.create("moveUp", GLFW_KEY_SPACE)->onPress.subscribe([&](auto state){ camera.moveUp(-getRealSpeed(state) * timeDelta); });
    keyboardInput.create("moveDown", GLFW_KEY_C)->onPress.subscribe([&](auto state){ camera.moveUp(getRealSpeed(state) * timeDelta); });
    keyboardInput.create("exit", GLFW_KEY_ESCAPE)->onPress.subscribe([&](auto){ window.close(); });
    auto toggleCursorMode = keyboardInput.create("toggleCursorMode", GLFW_KEY_M);
    toggleCursorMode->onPress.subscribe([&](auto) { window.toggleCursorMode(); });
    toggleCursorMode->setIsRepeatable(false);
    mouseInput.create("cameraView", 0)->onMove.subscribe([&](glm::vec2 delta, auto){ camera.yawAndPitch(delta * timeDelta * mouseSensitivity); });
    // clang-format on

    // ====================== MISC ==========================
    Core::FStopwatch modelLoaderStopwatch;
    modelLoaderStopwatch.start();

    std::vector<std::filesystem::path> modelPaths
        = { "assets/base-3d/Models/FBX/Tree.fbx", "assets/base-3d/Models/FBX/FireHydrant.fbx" };

    Assimp::Importer importer;
    for (auto&& path : modelPaths)
    {
        const aiScene* scene = importer.ReadFile(
            path.generic_string().c_str(),
            aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType);
        if (Verify(scene))
        {
            for (int i = 0; i < scene->mNumMeshes; ++i)
            {
                if (aiMesh* rawMesh = scene->mMeshes[i]; Verify(rawMesh))
                {
                    auto mesh = StaticMeshFactory::CreateBase(rawMesh->mName.C_Str());
                    mesh.importFrom(rawMesh, scene, path);
                    mesh.setShaderProgram(shader);
                    meshes.push_back(std::move(mesh));
                }
            }
        }
    }
    // double side render for tree's leaf
    meshes.at(5).setDrawModifiers({
        { GL_CULL_FACE, GraphicsComponentData::Modifier::Disable },
        { GL_BLEND, GraphicsComponentData::Modifier::Enable },
    });

    meshes.at(8).moveRight(100);
    globalLog.infoLog("All models was loaded for: {}s"_f << modelLoaderStopwatch.stop());

    //   ___  ___        _          _
    //   |  \/  |       (_)        | |
    //   | .  . |  __ _  _  _ __   | |      ___    ___   _ __
    //   | |\/| | / _` || || '_ \  | |     / _ \  / _ \ | '_ \
    //   | |  | || (_| || || | | | | |____| (_) || (_) || |_) |
    //   \_|  |_/ \__,_||_||_| |_| \_____/ \___/  \___/ | .__/
    //                                                  | |
    //                                                  |_|
    //-----------------------------------------------------------
    Core::FStopwatch clock;

    FPSCounter fpsCounter;
    fpsCounter.start();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    while (!window.shouldClose())
    {
        clock.start();

        keyboardInput.update();
        mouseInput.update();

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader->use();
        shader->setUniform("uObjectColor"_atom, 1.0f, 1.0f, 1.0f);
        shader->setUniform("uLightColor"_atom, 1.0f, 1.0f, 1.0f);
        shader->setUniform("uLightPos"_atom, lightPos);
        shader->setUniform("uViewPos"_atom, camera.getPosition());
        shader->setUniform("uTexture"_atom, 0);

        shader->setUniform("uProjAndView"_atom, camera.getMatrix());

        meshes.at(4).directDraw();
        meshes.at(5).directDraw();
        meshes.at(8).directDraw();

        window.swapBuffers();
        window.pollEvent();

        fpsCounter.newFrameUpdate();
        timeDelta = clock.stop();
    }

    globalLog.infoLog("FPS: {}"_f << fpsCounter.getFPS());

    return 0;
}
