#include "RawGraphics/ShaderProgram.h"
#include "RawGraphics/Window.h"

#include <iostream>

int main()
{
#ifdef DEBUG
    spdlog::set_level(spdlog::level::trace);
#endif

    constexpr Core::ISize2 windowSize{ 600, 600 };
    Core::ISize2 viewportSize = windowSize;

    glm::ivec2 pos{ 200, 0 };
    viewportSize.width -= pos.x;

    SW::GetWindow().create("Sprite Walker", windowSize);

    SW::ShaderProgram shader;
    shader.setShader(SW::VertexShader("assets/shaders/color.vert"));
    shader.setShader(SW::FragmentShader("assets/shaders/color.frag"));
    shader.create("color"_atom);

    float vertices[] = {
        0.5f,  0.5f,  0.0f, // top right
        0.5f,  -0.5f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f, // bottom left
        -0.5f, 0.5f,  0.0f  // top left
    };
    unsigned int indices[] = {
        0, 1, 3, // first Triangle
        1, 2, 3  // second Triangle
    };
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), static_cast<void*>(0));
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

    glViewport(pos.x, pos.y, viewportSize.width, viewportSize.height);

    while (!SW::GetWindow().shouldClose())
    {
        glEnable(GL_SCISSOR_TEST);
        glScissor(pos.x, pos.y, viewportSize.width, viewportSize.height);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glDisable(GL_SCISSOR_TEST);

        glEnable(GL_SCISSOR_TEST);
        glScissor(0, 0, pos.x, windowSize.height);
        glClearColor(1.f, 1.f, 1.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);
        glDisable(GL_SCISSOR_TEST);

        shader.use();
        glBindVertexArray(VAO);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        SW::GetWindow().swapBuffers();
        SW::GetWindow().pollEvent();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    return 0;
}