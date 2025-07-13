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

#include "ShaderProgramMeta.h"

#include "Utils/Functions.h"

namespace SW
{

    std::size_t ShaderProgramMeta::Hasher::operator()(const ShaderProgramMeta& self) const
    {
        return self._shaderName.makeHash();
    }

    bool ShaderProgramMeta::operator==(const ShaderProgramMeta& other)
    {
        return _shaderName == other._shaderName;
    }

    void ShaderProgramMeta::create(const std::filesystem::path& vertexShaderPath,
                                   const std::filesystem::path& fragmentShaderPath)
    {
        generateShaderId();
        readSourceShaderFile(vertexShaderPath, fragmentShaderPath);
        compileShader();
        _shaderProgram.create(_shaderName);
    }

    void ShaderProgramMeta::compileShader()
    {
        glCompileShader(_shaderProgram.getVertexShader());
        glCompileShader(_shaderProgram.getFragmentShader());
        requireNoCompileErrors();
    }

    void ShaderProgramMeta::requireNoCompileErrors()
    {
        checkShaderCompileStatus(_shaderProgram.getVertexShader(), "Vertex");
        checkShaderCompileStatus(_shaderProgram.getFragmentShader(), "Fragment");
    }

    void ShaderProgramMeta::generateShaderId()
    {
        _shaderProgram.clear();

        auto vertexShaderId = glCreateShader(static_cast<GLenum>(ShaderType::Vertex));
        if (vertexShaderId == 0)
        {
            criticalThrowingLog("Can't create gl vertex shader.");
        }
        _shaderProgram.setVertexShaderId(vertexShaderId);

        auto fragmentShaderId = glCreateShader(static_cast<GLenum>(ShaderType::Fragment));
        if (fragmentShaderId == 0)
        {
            criticalThrowingLog("Can't create gl fragment shader.");
        }
        _shaderProgram.setFragmentShaderId(fragmentShaderId);
    }

    void ShaderProgramMeta::readSourceShaderFile(const std::filesystem::path& vertexShaderPath,
                                                 const std::filesystem::path& fragmentShaderPath)
    {
        // VERTEX SHADER
        const auto vertexSources = Utils::GetTextFileContentAs<std::string>(vertexShaderPath);
        if (vertexSources.empty())
        {
            return;
        }
        const auto* vertexRaw = vertexSources.data();
        glShaderSource(_shaderProgram.getVertexShader(), 1, &vertexRaw, nullptr);

        // FRAGMENT SAHDER
        const auto fragmentSources = Utils::GetTextFileContentAs<std::string>(fragmentShaderPath);
        if (fragmentSources.empty())
        {
            return;
        }
        const auto* fragmentRaw = fragmentSources.data();
        glShaderSource(_shaderProgram.getFragmentShader(), 1, &fragmentRaw, nullptr);
    }

    void ShaderProgramMeta::setShaderName(const Core::StringAtom& name)
    {
        _shaderName = Core::StringAtom::Intern(name);
    }

    void ShaderProgramMeta::setShaderName(const std::string& name)
    {
        _shaderName = Core::StringAtom::Intern(name);
    }

    void ShaderProgramMeta::checkShaderCompileStatus(GLuint shaderId, const std::string& shaderType)
    {
        constexpr GLsizei logSize = 512;
        GLint success = 0;
        char infoLog[logSize];

        glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shaderId, logSize, nullptr, infoLog);
            std::string msg = shaderType + " shader compilation error: ";
            msg += infoLog;
            throw std::runtime_error(msg);
        }
    }

} // namespace SW
