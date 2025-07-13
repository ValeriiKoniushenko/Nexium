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
    }

    void ShaderProgramMeta::clearShaderId()
    {
        if (_vertexShaderId != 0)
        {
            glDeleteShader(_vertexShaderId);
            _vertexShaderId = 0;
        }
        if (_fragmentShaderId != 0)
        {
            glDeleteShader(_fragmentShaderId);
            _fragmentShaderId = 0;
        }
    }

    void ShaderProgramMeta::compileShader()
    {
        glCompileShader(_vertexShaderId);
        glCompileShader(_fragmentShaderId);
        requireNoCompileErrors();
    }

    void ShaderProgramMeta::requireNoCompileErrors()
    {
        checkShaderCompileStatus(_vertexShaderId, "Vertex");
        checkShaderCompileStatus(_fragmentShaderId, "Fragment");
    }

    void ShaderProgramMeta::generateShaderId()
    {
        clearShaderId();

        _vertexShaderId = glCreateShader(static_cast<GLenum>(ShaderType::Vertex));
        if (_vertexShaderId == 0)
        {
            throw std::runtime_error("Can't create gl vertex shader.");
        }

        _fragmentShaderId = glCreateShader(static_cast<GLenum>(ShaderType::Fragment));
        if (_fragmentShaderId == 0)
        {
            throw std::runtime_error("Can't create gl fragment shader.");
        }
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
        glShaderSource(_vertexShaderId, 1, &vertexRaw, nullptr);

        // FRAGMENT SAHDER
        const auto fragmentSources = Utils::GetTextFileContentAs<std::string>(fragmentShaderPath);
        if (fragmentSources.empty())
        {
            return;
        }
        const auto* fragmentRaw = fragmentSources.data();
        glShaderSource(_fragmentShaderId, 1, &fragmentRaw, nullptr);
    }

    void ShaderProgramMeta::setShaderName(const Core::StringAtom& name)
    {
        _shaderName = Core::StringAtom::Intern(name);
    }

    void ShaderProgramMeta::setShaderName(const std::string& name)
    {
        _shaderName = Core::StringAtom::Intern(name);
    }

    ShaderProgram ShaderProgramMeta::generateShaderProgram()
    {
        ShaderProgram shaderProgram;
        shaderProgram.setVertexShaderId(_vertexShaderId);
        shaderProgram.setFragmentShaderId(_fragmentShaderId);
        shaderProgram.create(_shaderName);

        return shaderProgram;
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
