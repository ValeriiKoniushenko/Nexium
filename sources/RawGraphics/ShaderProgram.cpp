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

    void ShaderProgram::create(const Core::StringAtom& shaderName)
    {
        clear();

        debugLog("Creating of the shader program '{}' is started."_f << shaderName);

        _name = shaderName;
        _name.shrink_to_fit();

        if (_vertexShader.isEmpty())
        {
            criticalThrowingLog("Vertex shader is empty. Impossible to create the shader program."_f
                                << shaderName);
        }
        if (_fragmentShader.isEmpty())
        {
            criticalThrowingLog("Vertex shader is empty. Impossible to create the shader program."_f
                                << shaderName);
        }

        _data = glCreateProgram();
        if (_data == 0)
        {
            criticalThrowingLog("glCreateProgram to create a shader program was failed for '{}'"_f
                                << shaderName);
        }

        glAttachShader(_data, _vertexShader.data());
        glAttachShader(_data, _fragmentShader.data());
        glLinkProgram(_data);

        int success = 0;
        glGetProgramiv(_data, GL_LINK_STATUS, &success);
        if (!success)
        {
            clear();

            constexpr auto size = 512;
            char infoLog[size];
            glGetProgramInfoLog(_data, 512, nullptr, infoLog);
            criticalThrowingLog("Shader program compilation error: "_f << infoLog);
        }

        debugLog("Finished: creating of the shader program '{}'"_f << shaderName);
    }

    void ShaderProgram::clear()
    {
        if (_data != 0)
        {
            glDeleteProgram(_data);
            _data = 0;
        }
        _name.clear();
        _vertexShader.clear();
        _fragmentShader.clear();
    }

} // namespace SW