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

#include "ShaderProgram.h"

namespace SW
{

    ShaderProgram& ShaderProgram::operator=(ShaderProgram&& other) noexcept
    {
        if (this != &other) [[likely]]
        {
            _fragmentShaderId = other._fragmentShaderId;
            _vertexShaderId = other._vertexShaderId;
            _name = std::move(other._name);
            _shaderProgramId = other._shaderProgramId;

            other._fragmentShaderId = 0;
            other._vertexShaderId = 0;
            other._shaderProgramId = 0;
        }
        return *this;
    }

    void ShaderProgram::setName(const Core::StringAtom& name)
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

    void ShaderProgram::create(const Core::StringAtom& shaderName)
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

        reflectShaderVariables();

#ifdef GRAPHICS_DEBUG
        auto printVariables
            = [this](const char* groupName,
                     const std::unordered_set<ShaderVariable, ShaderVariableHash>& vars)
        {
            infoLog("Shader [{}] {} variables:"_f << _name << groupName);
            for (const auto& var : vars)
            {
                infoLog("    name: '{}', type: {}, size: {}, location: {}"_f
                        << var.name << (int)var.type << var.size << var.location);
            }
        };

        printVariables("UNIFORMS", _uniforms);
        printVariables("INPUTS", _inputs);
        printVariables("OUTPUTS", _outputs);

#endif

        infoLog("Finished: creating of the shader program '{}'"_f << shaderName);
    }

    void ShaderProgram::clear()
    {
        clearOnlyShaderProgram();
        _name.clear();
        _vertexShaderId = 0;
        _fragmentShaderId = 0;
    }

    void ShaderProgram::clearOnlyShaderProgram()
    {
        if (_shaderProgramId != 0)
        {
            glDeleteProgram(_shaderProgramId);
            _shaderProgramId = 0;
        }
    }

    void ShaderProgram::reflectShaderVariables()
    {
        if (!glGetProgramInterfaceiv)
        {
            warnLog("The function: glGetProgramInterfaceiv - is unavailable.");
            return;
        }
        _uniforms.clear();
        _inputs.clear();
        _outputs.clear();

        struct Group
        {
            GLenum interfaceType;
            std::unordered_set<ShaderVariable, ShaderVariableHash>& output;
        };

        std::vector<Group> groups = { { GL_UNIFORM, _uniforms },
                                      { GL_PROGRAM_INPUT, _inputs },
                                      { GL_PROGRAM_OUTPUT, _outputs } };

        constexpr GLenum props[] = { GL_NAME_LENGTH, GL_TYPE, GL_LOCATION, GL_ARRAY_SIZE };

        for (const auto& group : groups)
        {
            GLint count = 0;
            glGetProgramInterfaceiv(_shaderProgramId, group.interfaceType, GL_ACTIVE_RESOURCES,
                                    &count);

            for (GLint i = 0; i < count; ++i)
            {
                GLint values[4] = {};
                glGetProgramResourceiv(_shaderProgramId, group.interfaceType, i, 4, props, 4,
                                       nullptr, values);

                GLint nameLen = values[0];
                GLenum type = values[1];
                GLint location = values[2];
                GLint size = values[3];

                std::string name(nameLen, '\0');
                glGetProgramResourceName(_shaderProgramId, group.interfaceType, i, nameLen, nullptr,
                                         name.data());
                if (!name.empty() && name.back() == '\0')
                {
                    name.pop_back();
                }

                ShaderVariable var{ Core::StringAtom::Intern(name), type, size, location };
                group.output.insert(std::move(var));
            }
        }
    }

} // namespace SW
