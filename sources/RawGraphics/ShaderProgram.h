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

#include "ModuleInfo.h"

// clang-format off
#include "glad/glad.h"
#include "GLFW/glfw3.h"
// clang-format on

#include <unordered_set>

namespace SW
{

    struct ShaderVariable
    {
        Core::StringAtom name;
        GLenum type = 0;
        GLint size = 0;
        GLint location = 0;

        [[nodiscard]] bool operator==(const ShaderVariable& other) const
        {
            return name == other.name;
        }
    };

    struct ShaderVariableHash
    {
        std::size_t operator()(const ShaderVariable& obj) const { return obj.name.makeHash(); }
    };

    class ShaderProgram : public BaseLog
    {
    public:
        ~ShaderProgram() override = default;
        ShaderProgram() = default;
        ShaderProgram(ShaderProgram&& other) noexcept { *this = std::move(other); }
        ShaderProgram& operator=(ShaderProgram&& other) noexcept;

        void setVertexShaderId(GLuint shader) { _vertexShaderId = shader; }
        void setFragmentShaderId(GLuint shader) { _fragmentShaderId = shader; }
        void setName(const Core::StringAtom& name);

        [[nodiscard]] GLuint getFragmentShader() noexcept { return _fragmentShaderId; }
        [[nodiscard]] GLuint getVertexShader() noexcept { return _vertexShaderId; }

        [[nodiscard]] bool isEmpty() const noexcept { return _shaderProgramId == 0; }

        [[nodiscard]] GLuint getShaderProgramId() const noexcept { return _shaderProgramId; }

        void create(const Core::StringAtom& shaderName);
        void clear();
#ifdef GRAPHICS_DEBUG
        void use() const noexcept
        {
            if (isEmpty())
            {
                criticalThrowingLog("Impossible to use not-created shader program.");
            }
            glUseProgram(_shaderProgramId);
        }
#else
        void use() const noexcept { glUseProgram(_data); }
#endif

        [[nodiscard]] spdlog::logger* getLogger() const override
        {
            return RawGraphics::getLogger();
        }

        [[nodiscard]] const char* getPrefix() const override { return "ShaderProgram"; }

        void __setUniformsSource(
            const std::unordered_set<ShaderVariable, ShaderVariableHash>* source);

    protected:
        void clearOnlyShaderProgram();

    protected:
        // non-owner, read-only
        const std::unordered_set<ShaderVariable, ShaderVariableHash>* _uniforms = nullptr;

        Core::StringAtom _name;

        GLuint _vertexShaderId = 0;
        GLuint _fragmentShaderId = 0;
        GLuint _shaderProgramId = 0;
    };

} // namespace SW
