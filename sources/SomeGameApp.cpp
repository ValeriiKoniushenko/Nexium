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
#include "RawGraphics/GraphicsComponents.h"
#include "RawGraphics/ShaderManager.h"
#include "RawGraphics/ShaderProgramMeta.h"
#include "RawGraphics/Window.h"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "glm/gtc/type_ptr.hpp"

#include <iostream>

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

    auto& sm = SW::GetShaderManager();
    sm.loadShader("assets/shaders/");
    auto shader = sm.getShaderProgram("color");

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
    // element.setShaderProgram();

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), static_cast<void*>(0));
    glEnableVertexAttribArray(0);

    glm::mat4 model = glm::mat4(1.0f);

    const auto uProjAndView = glGetUniformLocation(shader.getShaderProgramId(), "uProjAndView");
    const auto uModel = glGetUniformLocation(shader.getShaderProgramId(), "uModel");

    int frames = 0;
    const auto start = std::chrono::system_clock::now();

    SW::BaseCamera camera;
    camera.moveForward(-10);

    while (!window.shouldClose())
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUniformMatrix4fv(uProjAndView, 1, GL_FALSE, glm::value_ptr(camera.getMatrix()));
        glUniformMatrix4fv(uModel, 1, GL_FALSE, glm::value_ptr(model));
        element.directDraw();

        window.swapBuffers();
        window.pollEvent();
        ++frames;
    }

    const auto end = std::chrono::system_clock::now();
    const auto diff = std::chrono::duration<double>(end - start).count();
    std::cout << "FPS: " << static_cast<double>(frames) / diff << std::endl;

    return 0;
}
