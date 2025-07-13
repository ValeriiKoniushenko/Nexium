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

#pragma once

#include "Core/String.h"
#include "ShaderProgram.h"

namespace SW
{

    // clang-format off
    enum class ShaderType : GLenum
    {
        None = 0,
        Fragment = GL_FRAGMENT_SHADER,
        Vertex = GL_VERTEX_SHADER,
        Geometry = GL_GEOMETRY_SHADER
    };
    // clang-format on

    enum class ShaderStorageQualifierType
    {
        Input,
        Output,
        Uniform
    };

    struct ShaderVariable
    {
        std::string name;
        GLenum type;
        GLint size;
        GLint location;
    };

    class ShaderProgramMeta final
    {
    public:
        struct Hasher
        {
            [[nodiscard]] std::size_t operator()(const ShaderProgramMeta& self) const;
        };

    public:
        ShaderProgramMeta() = default;

        [[nodiscard]] bool operator==(const ShaderProgramMeta& other);

        void create(const std::filesystem::path& vertexShaderPath,
                    const std::filesystem::path& fragmentShaderPath);

        void compileShader();
        void requireNoCompileErrors();

        void clearShaderId();
        void generateShaderId();
        void readSourceShaderFile(const std::filesystem::path& vertexShaderPath,
                                  const std::filesystem::path& fragmentShaderPath);

        void setShaderName(const std::string& name);
        void setShaderName(const Core::StringAtom& name);

        [[nodiscard]] ShaderProgram generateShaderProgram();
        [[nodiscard]] Core::StringAtom getShaderName() const { return _shaderName; }

    private:
        void checkShaderCompileStatus(GLuint shaderId, const std::string& shaderType);

    private:
        GLuint _vertexShaderId = {};
        GLuint _fragmentShaderId = {};
        Core::StringAtom _shaderName;

        // VertexShader _vertexShader;
        // FragmentShader _fragmentShader;
    };
} // namespace SW
