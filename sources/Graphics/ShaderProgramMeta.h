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

#pragma once

#include "Core/String.h"
#include "ShaderProgram.h"

namespace Core
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

    class ShaderProgramMeta final : public BaseLog
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

        [[nodiscard]] ShaderProgram& getShaderProgram() noexcept { return _shaderProgram; }

        [[nodiscard]] const ShaderProgram& getShaderProgram() const noexcept
        {
            return _shaderProgram;
        }

        [[nodiscard]] const std::unordered_set<ShaderVariable, ShaderVariable::Hasher>&
            getUniforms() const
        {
            return _uniforms;
        }

        [[nodiscard]] const std::unordered_set<ShaderVariable, ShaderVariable::Hasher>& getInputs()
            const
        {
            return _inputs;
        }

        [[nodiscard]] const std::unordered_set<ShaderVariable, ShaderVariable::Hasher>& getOutputs()
            const
        {
            return _outputs;
        }

        void setShaderName(const std::string& name);

        void setShaderName(const StringAtom& name);

        [[nodiscard]] const StringAtom& getShaderName() const { return _shaderName; }

        [[nodiscard]] spdlog::logger* getLogger() const override { return Graphics::getLogger(); }

        [[nodiscard]] const char* getPrefix() const override { return "ShaderProgramMeta"; }

        void recreateFromSources();

        bool safeRecreateFromSources();

        [[nodiscard]] const std::filesystem::path& getVertexShaderPath() const noexcept
        {
            return _vertexShaderPath;
        }

        [[nodiscard]] const std::filesystem::path& getFragmentShaderPath() const noexcept
        {
            return _fragmentShaderPath;
        }

    private:
        void compileShader();

        void requireNoCompileErrors();

        void generateShaderId();

        void readSourceShaderFile(const std::filesystem::path& vertexShaderPath,
                                  const std::filesystem::path& fragmentShaderPath);

        static void checkShaderCompileStatus(GLuint shaderId, const std::string& shaderType);

        void reflectShaderVariablesFor(GLuint shaderProgramId);

    private:
        ShaderProgram _shaderProgram;
        StringAtom _shaderName;

        std::unordered_set<ShaderVariable, ShaderVariable::Hasher> _uniforms;
        std::unordered_set<ShaderVariable, ShaderVariable::Hasher> _inputs;
        std::unordered_set<ShaderVariable, ShaderVariable::Hasher> _outputs;

        std::filesystem::path _vertexShaderPath;
        std::filesystem::path _fragmentShaderPath;
    };
} // namespace Core