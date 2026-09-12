// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "Line.h"

#include "../GraphicsComponents.h"
#include "../ShaderManager.h"
#include "../ShaderProgram.h"

using namespace NX;
using namespace Core;

namespace
{
    struct LineVertex
    {
        glm::vec3 position;
    };

    BaseGraphicsData GenerateDefaultLineBaseGraphicsData()
    {
        using namespace Core;

        BaseGraphicsData data;
        data.generate();

        auto* lineShader = GetShaderManager().getShaderProgram("2d_rect"_atom);
        if (!lineShader)
        {
            LOG_ASSERT_ERROR_ONCE_P(gGlobalLog, "Can't get shader program '2d_rect'.");
            return {};
        }
        data.setShader(lineShader);

        data.bindAllBuffers();
        glBufferData(GL_ARRAY_BUFFER, sizeof(LineVertex) * 2, nullptr, GL_STREAM_DRAW);
        lineShader->callSetEvent(ShaderProgram::Event::OnSetIndexAndVertexBuffer);

        return data;
    }
} // namespace

namespace NX::Debug
{
    void Line::Draw(const ShaderProgram* shader, const glm::mat4& projViewMatric,
                    const glm::vec3& start, const glm::vec3& end, float thickness,
                    const NormColor4& color)
    {
        if (!shader) [[likely]]
        {
            LOG_ASSERT_ERROR_ONCE_P(gGlobalLog, "Can't draw a line. Shader is empty.");
            return;
        }

        static BaseGraphicsData gcd = GenerateDefaultLineBaseGraphicsData();

        shader->use();
        shader->setUniform("uProjAndView"_atom, projViewMatric);
        shader->setUniform("uColor"_atom, color);

        gcd.bindAllBuffers();

        const std::array<LineVertex, 2> vertices = { LineVertex{ start }, LineVertex{ end } };
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices.data());

        glLineWidth(thickness);
        glDrawArrays(GL_LINES, 0, 2);
    }

    void Line::Draw(const ShaderProgram* shader, const glm::mat4& projViewMatric, glm::vec2 start,
                    glm::vec2 end, float thickness, const NormColor4& color)
    {
        Draw(shader, projViewMatric, glm::vec3(start, 0.f), glm::vec3(end, 0.f), thickness, color);
    }

} // namespace NX::Debug