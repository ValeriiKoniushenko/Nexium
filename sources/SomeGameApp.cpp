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
#include "InputDevices/Keyboard.h"
#include "RawGraphics/GraphicsComponents.h"
#include "RawGraphics/ShaderManager.h"
#include "RawGraphics/ShaderProgramMeta.h"
#include "RawGraphics/Window.h"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/string_cast.hpp"

#include <iostream>
#include <thread>

int main()
{
#ifdef DEBUG
    spdlog::set_level(spdlog::level::trace);
#endif
    spdlog::set_pattern("%D [%L] [%n] %v");

    // SW::EditorServer server;
    // server.initialize();
    // server.start();

    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(
        "assets/base-3d/cube.obj", aiProcess_CalcTangentSpace | aiProcess_Triangulate
                                       | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType);

    auto& window = SW::GetWindow();
    window.create("Sprite Walker", { 600, 600 });
    // SW::ShaderProgram shader;
    //  shader.setShader(SW::FragmentShader("assets/shaders/color.frag"));
    //  shader.setShader(SW::VertexShader("assets/shaders/color.vert"));
    //  shader.create("color"_atom);

    auto& sm = SW::ShaderManager::instance();
    sm.loadShader("assets/shaders");
    auto* shader = sm.getShaderProgram("color"_atom);
    Assert(shader);

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

    glm::mat4 model = glm::mat4(1.0f);

    const auto uProjAndView = glGetUniformLocation(shader->getShaderProgramId(), "uProjAndView");
    const auto uModel = glGetUniformLocation(shader->getShaderProgramId(), "uModel");

    int frames = 0;
    const auto start = std::chrono::system_clock::now();

    SW::BaseCamera camera;
    camera.moveForward(-10);

    float speed = 10.f;
    float rotateSpeed = 15.f;

    std::chrono::time_point<std::chrono::system_clock> frameStart;
    float timeDelta = 0.f;
    while (!window.shouldClose())
    {
        frameStart = std::chrono::system_clock::now();
        if (SW::Keyboard::isKeyPressed(GLFW_KEY_D))
        {
            camera.moveRight(speed * timeDelta);
            std::cout << "Camera XYZ: " << glm::to_string(camera.getPosition()) << std::endl;
        }
        if (SW::Keyboard::isKeyPressed(GLFW_KEY_A))
        {
            camera.moveRight(-speed * timeDelta);
            std::cout << "Camera XYZ: " << glm::to_string(camera.getPosition()) << std::endl;
        }
        if (SW::Keyboard::isKeyPressed(GLFW_KEY_W))
        {
            camera.moveUp(speed * timeDelta);
            std::cout << "Camera XYZ: " << glm::to_string(camera.getPosition()) << std::endl;
        }
        if (SW::Keyboard::isKeyPressed(GLFW_KEY_S))
        {
            camera.moveUp(-speed * timeDelta);
            std::cout << "Camera XYZ: " << glm::to_string(camera.getPosition()) << std::endl;
        }
        if (SW::Keyboard::isKeyPressed(GLFW_KEY_E))
        {
            std::cout << "Rotation: " << camera.getRotationY() << std::endl;
            camera.rotateY(rotateSpeed * timeDelta);
        }
        if (SW::Keyboard::isKeyPressed(GLFW_KEY_Q))
        {
            std::cout << "Rotation: " << camera.getRotationY() << std::endl;
            camera.rotateY(-rotateSpeed * timeDelta);
        }

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUniformMatrix4fv(uProjAndView, 1, GL_FALSE, glm::value_ptr(camera.getMatrix()));
        glUniformMatrix4fv(uModel, 1, GL_FALSE, glm::value_ptr(model));
        element.directDraw();

        window.swapBuffers();
        window.pollEvent();
        ++frames;

        // using namespace std::chrono_literals;
        // std::this_thread::sleep_for(10ms);

        timeDelta
            = std::chrono::duration<double>(std::chrono::system_clock::now() - frameStart).count();
    }

    const auto end = std::chrono::system_clock::now();
    const auto diff = std::chrono::duration<double>(end - start).count();
    std::cout << "FPS: " << static_cast<double>(frames) / diff << std::endl;

    return 0;
}
