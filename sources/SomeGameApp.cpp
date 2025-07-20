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
#include "InputDevices/InputManager.h"
#include "Misc/FPSCounter.h"
#include "RawGraphics/GraphicsComponents.h"
#include "RawGraphics/ShaderManager.h"
#include "RawGraphics/Window.h"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/string_cast.hpp"

#include <iostream>

int main()
{
#ifdef DEBUG
    spdlog::set_level(spdlog::level::trace);
#endif
    spdlog::set_pattern("%D [%L] [%n] %v");

    //    _____
    //   /  ___|
    //   \ `--.   ___  _ __ __   __ ___  _ __
    //    `--. \ / _ \| '__|\ \ / // _ \| '__|
    //   /\__/ /|  __/| |    \ V /|  __/| |
    //   \____/  \___||_|     \_/  \___||_|
    //------------------------------------------
    auto& server = SW::GetEditorServer();
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
    auto& window = SW::GetWindow();
    window.create("Sprite Walker", { 600, 600 });

    //    _____  _                 _
    //   /  ___|| |               | |
    //   \ `--. | |__    __ _   __| |  ___  _ __  ___
    //    `--. \| '_ \  / _` | / _` | / _ \| '__|/ __|
    //   /\__/ /| | | || (_| || (_| ||  __/| |   \__ \
    //   \____/ |_| |_| \__,_| \__,_| \___||_|   |___/
    //-------------------------------------------------
    auto& sm = SW::ShaderManager::instance();
    sm.loadShaders("assets/shaders");
    sm.debugLog("Was loaded {} shaders."_f << sm.countOfShaders());
    for (const auto& notLoadedShader : sm.getFailedShaders())
    {
        sm.warnLog("Shader '{}' found but not loaded. It contains some error[s]."_f
                   << notLoadedShader);
    }

    // ======== Setting up[s] ==========
    auto* shader = sm.getShaderProgram("color"_atom);
    shader->setVertexAttributeCallback(
        []()
        {
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
        });
    Assert(shader);

    //    _    _               _      _     _   _
    //   | |  | |             | |    | |   | | | |
    //   | |  | |  ___   _ __ | |  __| |   | | | |  __ _  _ __  ___
    //   | |/\| | / _ \ | '__|| | / _` |   | | | | / _` || '__|/ __|
    //   \  /\  /| (_) || |   | || (_| |   \ \_/ /| (_| || |   \__ \
    //    \/  \/  \___/ |_|   |_| \__,_|    \___/  \__,_||_|   |___/
    //---------------------------------------------------------------
    float timeDelta = 0.f;
    SW::BaseCamera camera;
    camera.moveForward(-10);

    //    _____                       _        ___         _    _
    //   |_   _|                     | |      / _ \       | |  (_)
    //     | |   _ __   _ __   _   _ | |_    / /_\ \  ___ | |_  _   ___   _ __   ___
    //     | |  | '_ \ | '_ \ | | | || __|   |  _  | / __|| __|| | / _ \ | '_ \ / __|
    //    _| |_ | | | || |_) || |_| || |_    | | | || (__ | |_ | || (_) || | | |\__ \
    //    \___/ |_| |_|| .__/  \__,_| \__|   \_| |_/ \___| \__||_| \___/ |_| |_||___/
    //                 | |
    //                 |_|
    //--------------------------------------------------------------------------------
    SW::KeyboardInputManger keyboardInput;
    SW::MouseInputManger mouseInput;

    // clang-format off
    constexpr float speed = 10.f, rotateSpeed = 25.f, mouseSensitivity = 900.0;
    auto lShift = keyboardInput.create("lShift", GLFW_KEY_LEFT_SHIFT);
    lShift->setIsRepeatable(false);
    lShift->onPress.subscribe([&](){ std::cout << "Pressed\n"; });

    keyboardInput.create("moveForward", GLFW_KEY_W)->onPress.subscribe([&](){ camera.moveForward(speed * timeDelta); });
    keyboardInput.create("moveBackward", GLFW_KEY_S)->onPress.subscribe([&](){ camera.moveForward(-speed * timeDelta); });
    keyboardInput.create("moveRight", GLFW_KEY_D)->onPress.subscribe([&](){ camera.moveRight(speed * timeDelta); });
    keyboardInput.create("moveLeft", GLFW_KEY_A)->onPress.subscribe([&](){ camera.moveRight(-speed * timeDelta); });
    keyboardInput.create("moveUp", GLFW_KEY_SPACE)->onPress.subscribe([&](){ camera.moveUp(speed * timeDelta); });
    keyboardInput.create("moveDown", GLFW_KEY_C)->onPress.subscribe([&](){ camera.moveUp(-speed * timeDelta); });
    keyboardInput.create("yawForward", GLFW_KEY_E)->onPress.subscribe([&](){ camera.yaw(rotateSpeed* timeDelta); });
    keyboardInput.create("yawBackward", GLFW_KEY_Q)->onPress.subscribe([&](){ camera.yaw(-rotateSpeed* timeDelta); });
    keyboardInput.create("pitchForward", GLFW_KEY_F)->onPress.subscribe([&](){ camera.pitch(rotateSpeed* timeDelta); });
    keyboardInput.create("pitchBackward", GLFW_KEY_R)->onPress.subscribe([&](){ camera.pitch(-rotateSpeed* timeDelta); });
    keyboardInput.create("resetCameraRotate", GLFW_KEY_T)->onPress.subscribe([&](){ camera.setRotation({}); });
    keyboardInput.create("exit", GLFW_KEY_ESCAPE)->onPress.subscribe([&](){ window.close(); });
    keyboardInput.create("increaseFOV", GLFW_KEY_J)->onPress.subscribe([&](){ camera.setFov(camera.getFov() + 100.f * timeDelta); });
    keyboardInput.create("decreaseFOV", GLFW_KEY_K)->onPress.subscribe([&](){ camera.setFov(camera.getFov() + -100.f * timeDelta); });
    auto toggleCursorMode = keyboardInput.create("toggleCursorMode", GLFW_KEY_M);
    toggleCursorMode->onPress.subscribe([&]() { window.toggleCursorMode(); });
    toggleCursorMode->setIsRepeatable(false);
    mouseInput.create("cameraView", 0)->onMove.subscribe([&](glm::vec2 delta){ camera.yawAndPitch(delta * timeDelta * mouseSensitivity); });
    // clang-format on

    // ====================== MISC ==========================
    Core::FStopwatch modelLoaderStopwatch;
    modelLoaderStopwatch.start();
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile("assets/base-3d/FireHydrant.fbx",
                                             aiProcess_Triangulate | aiProcess_JoinIdenticalVertices
                                                 | aiProcess_SortByPType);
    SW::globalLog.infoLog("All models was loaded for: {}s"_f << modelLoaderStopwatch.stop());

    std::vector<SW::GraphicsComponentData> meshes(scene->mNumMeshes);

    for (int i = 0; i < meshes.size(); ++i)
    {
        meshes[i].generate();
        meshes[i].setMesh(scene->mMeshes[i]);
        meshes[i].setShaderProgram(shader);
    }

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

    SW::FPSCounter fpsCounter;
    fpsCounter.start();

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    while (!window.shouldClose())
    {
        clock.start();

        keyboardInput.update();
        mouseInput.update();

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        shader->use();
        shader->setUniform("uProjAndView"_atom, camera.getMatrix());
        shader->setUniform("uModel"_atom, glm::mat4(1.0f));

        meshes[0].directDraw();

        window.swapBuffers();
        window.pollEvent();

        fpsCounter.newFrameUpdate();
        timeDelta = clock.stop();
    }

    SW::globalLog.infoLog("FPS: {}"_f << fpsCounter.getFPS());

    return 0;
}
