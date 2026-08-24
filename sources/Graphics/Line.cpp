/*
 * MIT License
 *
 * Copyright (c) 2018-2027 Valerii Koniushenko
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "Line.h"

#include "GameplaySystem/Framework/GameInstance.h"
#include "GraphicsComponents.h"
#include "ShaderProgram.h"

namespace
{
    struct LineVertex
    {
        glm::vec3 position;
    };

    Core::BaseGraphicsData GenerateDefaultLineBaseGraphicsData()
    {
        using namespace Core;

        BaseGraphicsData data;
        data.generate();

        auto* lineShader = GetShaderManager()->getShaderProgram("2d_rect"_atom);
        if (!lineShader)
        {
            LOG_ASSERT_ERROR_ONCE_P(globalLog, "Can't get shader program '2d_rect'.");
            return {};
        }
        data.setShader(lineShader);

        data.bindAllBuffers();
        glBufferData(GL_ARRAY_BUFFER, sizeof(LineVertex) * 2, nullptr, GL_STREAM_DRAW);
        lineShader->callSetEvent(ShaderProgram::Event::OnSetIndexAndVertexBuffer);

        return data;
    }
} // namespace

namespace Core::Debug
{
    void Line::Draw(const ShaderProgram* shader, const glm::mat4& projViewMatric,
                    const glm::vec3& start, const glm::vec3& end, const NormColor4& color)
    {
        if (!shader) [[likely]]
        {
            LOG_ASSERT_ERROR_ONCE_P(globalLog, "Can't draw a line. Shader is empty.");
            return;
        }

        static BaseGraphicsData gcd = GenerateDefaultLineBaseGraphicsData();

        shader->use();
        shader->setUniform("uProjAndView"_atom, projViewMatric);
        shader->setUniform("uColor"_atom, color);

        gcd.bindAllBuffers();

        const std::array<LineVertex, 2> vertices = { LineVertex{ start }, LineVertex{ end } };
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices.data());

        glDrawArrays(GL_LINES, 0, 2);
    }

    void Line::Draw(const ShaderProgram* shader, const glm::mat4& projViewMatric, glm::vec2 start,
                    glm::vec2 end, const NormColor4& color)
    {
        Draw(shader, projViewMatric, glm::vec3(start, 0.f), glm::vec3(end, 0.f), color);
    }

} // namespace Core::Debug