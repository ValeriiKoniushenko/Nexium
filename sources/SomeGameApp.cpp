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
#include "InputDevices/Keyboard.h"
#include "RawGraphics/GraphicsComponents.h"
#include "RawGraphics/ShaderManager.h"
#include "RawGraphics/ShaderProgramMeta.h"
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
    //-------------------------------------------------
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
    //-------------------------------------------------
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

    auto* shader = sm.getShaderProgram("color"_atom);
    Assert(shader);

    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(
        "assets/base-3d/cube.obj", aiProcess_CalcTangentSpace | aiProcess_Triangulate
                                       | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType);
    std::vector<float> vertices = {
        0.5f,  0.5f,  0.0f, // top right
        0.5f,  -0.5f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f, // bottom left
        -0.5f, 0.5f,  0.0f  // top left
    };
    std::vector<GLuint> indices = {
        0, 1, 3, // first Triangle
        1, 2, 3  // second Triangle
    };

    SW::GraphicsComponentData element;
    element.generate();
    element.setVertexBuffer(vertices);
    element.setIndexBuffer(indices);
    element.setShaderProgram(shader);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), static_cast<void*>(0));
    glEnableVertexAttribArray(0);

    int frames = 0;
    const auto start = std::chrono::system_clock::now();

    SW::BaseCamera camera;
    camera.moveForward(-10);

    float speed = 10.f;
    float rotateSpeed = 15.f;

    float timeDelta = 0.f;
    Core::FStopwatch clock;
    while (!window.shouldClose())
    {
        clock.start();
        if (SW::Keyboard::isKeyPressed(GLFW_KEY_D))
        {
            camera.moveRight(speed * timeDelta);
        }
        if (SW::Keyboard::isKeyPressed(GLFW_KEY_A))
        {
            camera.moveRight(-speed * timeDelta);
        }
        if (SW::Keyboard::isKeyPressed(GLFW_KEY_W))
        {
            camera.moveUp(speed * timeDelta);
        }
        if (SW::Keyboard::isKeyPressed(GLFW_KEY_S))
        {
            camera.moveUp(-speed * timeDelta);
        }
        if (SW::Keyboard::isKeyPressed(GLFW_KEY_E))
        {
            camera.rotateY(rotateSpeed * timeDelta);
        }
        if (SW::Keyboard::isKeyPressed(GLFW_KEY_Q))
        {
            camera.rotateY(-rotateSpeed * timeDelta);
        }

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        shader->use();
        shader->setUniform("uProjAndView"_atom, camera.getMatrix());
        shader->setUniform("uModel"_atom, glm::mat4(1.0f));

        element.directDraw();

        window.swapBuffers();
        window.pollEvent();
        ++frames;

        timeDelta = clock.stop();
    }

    const auto end = std::chrono::system_clock::now();
    const auto diff = std::chrono::duration<double>(end - start).count();
    const auto fps = frames / diff;
    std::cout << "FPS: " << fps << std::endl;

    return 0;
}
