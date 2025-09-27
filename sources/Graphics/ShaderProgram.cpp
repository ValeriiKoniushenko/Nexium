/*
 * MIT License
 *
 * Copyright (c) 2018-2025 Valerii Koniushenko
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

#include "ShaderProgram.h"

namespace Core
{
    ShaderProgram& ShaderProgram::operator=(ShaderProgram&& other) noexcept
    {
        if (this != &other) [[likely]]
        {
            _uniforms = std::move(other._uniforms);
            _name = std::move(other._name);
            _vertexShaderId = other._vertexShaderId;
            _fragmentShaderId = other._fragmentShaderId;
            _shaderProgramId = other._shaderProgramId;

            other._vertexShaderId = 0;
            other._fragmentShaderId = 0;
            other._shaderProgramId = 0;
        }
        return *this;
    }

    void ShaderProgram::setName(const StringAtom& name)
    {
        _name = name;

        if (!_name.isStatic())
        {
            _name.shrink_to_fit();
#ifdef GRAPHICS_DEBUG
            warnLog(
                "The shader's '{}' vairable '_name' is not atom. Make it atom for best performance."_f
                << _name);
#endif
        }
    }

    void ShaderProgram::create(const StringAtom& shaderName)
    {
        clearOnlyShaderProgram();

        debugLog("Creating of the shader program '{}' is started."_f << shaderName);

        setName(shaderName);

        if (_vertexShaderId == 0)
        {
            criticalThrowingLog("Vertex shader is empty. Impossible to create the shader program."_f
                                << shaderName);
        }
        if (_fragmentShaderId == 0)
        {
            criticalThrowingLog(
                "Fragment shader is empty. Impossible to create the shader program."_f
                << shaderName);
        }

        _shaderProgramId = glCreateProgram();
        if (_shaderProgramId == 0)
        {
            criticalThrowingLog("glCreateProgram to create a shader program was failed for '{}'"_f
                                << shaderName);
        }

        glAttachShader(_shaderProgramId, _vertexShaderId);
        glAttachShader(_shaderProgramId, _fragmentShaderId);
        glLinkProgram(_shaderProgramId);

        glDeleteShader(_vertexShaderId);
        glDeleteShader(_fragmentShaderId);

        int success = 0;
        glGetProgramiv(_shaderProgramId, GL_LINK_STATUS, &success);
        if (!success)
        {
            clear();

            constexpr auto size = 512;
            char infoLog[size];
            glGetProgramInfoLog(_shaderProgramId, 512, nullptr, infoLog);
            criticalThrowingLog("Shader program compilation error: "_f << infoLog);
        }

        infoLog("The shader program '{}' linked successfully."_f << shaderName);
    }

    void ShaderProgram::clear()
    {
        clearOnlyShaderProgram();
        _name.clear();
        _vertexShaderId = 0;
        _fragmentShaderId = 0;
    }

    void ShaderProgram::m__setUniformsFromSources(
        const std::unordered_set<ShaderVariable, ShaderVariable::Hasher>& source)
    {
        for (const auto& u : source)
        {
            _uniforms[u.name] = u.location;
        }
    }

    void ShaderProgram::setupVertexAttribute()
    {
        if (!_setupVertexAttribute) [[unlikely]]
        {
            ASSERT(false);
            errorLog("Impossible to setup setupVertexAttribute. Setup function wasn't found.");
            return;
        }

        _setupVertexAttribute();
    }

    void ShaderProgram::setVertexAttributeCallback(std::function<void()>&& func)
    {
        _setupVertexAttribute = std::forward<decltype(func)>(func);
    }

    void ShaderProgram::clearOnlyShaderProgram()
    {
        if (_shaderProgramId != 0)
        {
            glDeleteProgram(_shaderProgramId);
            _shaderProgramId = 0;
        }
    }

    void ShaderProgram::debugUniform(const StringAtom& name)
    {
#ifdef GRAPHICS_DEBUG
        ASSERT(name.isStatic(), "Use atomic string. Add _atom to your uniform's str");
        ASSERT(_uniforms[name] != -1);
#endif
    }
} // namespace Core