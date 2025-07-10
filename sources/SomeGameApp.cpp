
#include "RawGraphics/GraphicsComponents.h"
#include "RawGraphics/ShaderProgram.h"
#include "RawGraphics/ShaderProgramMeta.h"
#include "RawGraphics/Window.h"

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

    SW::GetWindow().create("Sprite Walker", { 600, 600 });

    SW::ShaderProgramMeta metaShader(SW::VertexShader("assets/shaders/color.vert"),
                                     SW::FragmentShader("assets/shaders/color.frag"), "color"_atom);

    auto shader = metaShader.generateShaderProgram();

    // SW::ShaderProgram shader;
    // shader.setShader(SW::VertexShader("assets/shaders/color.vert"));
    // shader.setShader(SW::FragmentShader("assets/shaders/color.frag"));
    // shader.create("color"_atom);

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
    while (!SW::GetWindow().shouldClose())
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        element.directDraw();

        SW::GetWindow().swapBuffers();
        SW::GetWindow().pollEvent();
        ++frames;
    }
    const auto end = std::chrono::system_clock::now();

    const auto diff = std::chrono::duration<double>(end - start).count();
    std::cout << "FPS: " << static_cast<double>(frames) / diff << std::endl;

    return 0;
}
