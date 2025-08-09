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
#include "OpenGL.h"

#include <glm/fwd.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <unordered_set>

namespace SW
{

    struct ShaderVariable
    {
        struct Hasher
        {
            std::size_t operator()(const ShaderVariable& obj) const { return obj.name.makeHash(); }
        };

        Core::StringAtom name;
        GLenum type = 0;
        GLint size = 0;
        GLint location = 0;

        [[nodiscard]] bool operator==(const ShaderVariable& other) const
        {
            return name == other.name;
        }
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
        [[nodiscard]] const Core::StringAtom& getName() const noexcept { return _name; }

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
        void use() const noexcept { glUseProgram(_shaderProgramId); }
#endif

        [[nodiscard]] spdlog::logger* getLogger() const override { return Graphics::getLogger(); }

        [[nodiscard]] const char* getPrefix() const override { return "ShaderProgram"; }

        // ================= UNIFORMs =====================
        // All uniform function below MUST be force inlined.
        // So, declare & implement it on spot.

        // clang-format off
        // Scalars
        void setUniform(const Core::StringAtom& name, GLfloat value) { glUniform1f(_uniforms[name], value); debugUniform(name); }
        void setUniform(const Core::StringAtom& name, GLint value) { glUniform1f(_uniforms[name], value); debugUniform(name); }
        void setUniform(const Core::StringAtom& name, GLuint value) { glUniform1f(_uniforms[name], value); debugUniform(name); }

        // vec2
        void setUniform(const Core::StringAtom& name, GLfloat x, GLfloat y) { glUniform2f(_uniforms[name], x,y);  debugUniform(name); }
        void setUniform(const Core::StringAtom& name, glm::vec2 v) { setUniform(name, v.x, v.y); }
        void setUniform(const Core::StringAtom& name, GLint x, GLint y) { glUniform2i(_uniforms[name], x,y);  debugUniform(name); }
        void setUniform(const Core::StringAtom& name, glm::ivec2 v) { setUniform(name, v.x, v.y); }
        void setUniform(const Core::StringAtom& name, GLuint x, GLuint y) { glUniform2ui(_uniforms[name], x,y);  debugUniform(name); }

        // vec3
        void setUniform(const Core::StringAtom& name, GLfloat x, GLfloat y, GLfloat z) { glUniform3f(_uniforms[name], x,y,z);  debugUniform(name); }
        void setUniform(const Core::StringAtom& name, glm::vec3 v) { setUniform(name, v.x, v.y, v.z); }
        void setUniform(const Core::StringAtom& name, GLint x, GLint y, GLint z) { glUniform3i(_uniforms[name], x,y,z);  debugUniform(name); }
        void setUniform(const Core::StringAtom& name, glm::ivec3 v) { setUniform(name, v.x, v.y, v.z); }
        void setUniform(const Core::StringAtom& name, GLuint x, GLuint y, GLuint z) { glUniform3ui(_uniforms[name], x,y,z);  debugUniform(name); }

        // vec4
        void setUniform(const Core::StringAtom& name, GLfloat x, GLfloat y, GLfloat z, GLfloat w) { glUniform4f(_uniforms[name], x,y,z,w);  debugUniform(name); }
        void setUniform(const Core::StringAtom& name, GLint x, GLint y, GLint z, GLint w) { glUniform4i(_uniforms[name], x,y,z,w);  debugUniform(name); }
        void setUniform(const Core::StringAtom& name, GLuint x, GLuint y, GLuint z, GLuint w) { glUniform4ui(_uniforms[name], x,y,z,w);  debugUniform(name); }

        void setUniform(const Core::StringAtom& name, const glm::mat2& value) { glUniformMatrix2fv(_uniforms[name], 1, GL_FALSE, glm::value_ptr(value)); debugUniform(name); }
        void setUniform(const Core::StringAtom& name, const glm::mat3& value) { glUniformMatrix3fv(_uniforms[name], 1, GL_FALSE, glm::value_ptr(value)); debugUniform(name); }
        void setUniform(const Core::StringAtom& name, const glm::mat4& value) { glUniformMatrix4fv(_uniforms[name], 1, GL_FALSE, glm::value_ptr(value)); debugUniform(name); }
        void setUniform(const Core::StringAtom& name, const glm::mat2x3& value) { glUniformMatrix2x3fv(_uniforms[name], 1, GL_FALSE, glm::value_ptr(value)); debugUniform(name); }
        void setUniform(const Core::StringAtom& name, const glm::mat3x2& value) { glUniformMatrix3x2fv(_uniforms[name], 1, GL_FALSE, glm::value_ptr(value)); debugUniform(name); }
        void setUniform(const Core::StringAtom& name, const glm::mat2x4& value) { glUniformMatrix2x4fv(_uniforms[name], 1, GL_FALSE, glm::value_ptr(value)); debugUniform(name); }
        void setUniform(const Core::StringAtom& name, const glm::mat4x2& value) { glUniformMatrix4x2fv(_uniforms[name], 1, GL_FALSE, glm::value_ptr(value)); debugUniform(name); }
        void setUniform(const Core::StringAtom& name, const glm::mat3x4& value) { glUniformMatrix3x4fv(_uniforms[name], 1, GL_FALSE, glm::value_ptr(value)); debugUniform(name); }
        void setUniform(const Core::StringAtom& name, const glm::mat4x3& value) { glUniformMatrix4x3fv(_uniforms[name], 1, GL_FALSE, glm::value_ptr(value)); debugUniform(name); }
        // clang-format on

        void __setUniformsFromSources(
            const std::unordered_set<ShaderVariable, ShaderVariable::Hasher>& source);

        void setupVertexAttribute();
        void setVertexAttributeCallback(std::function<void()>&& func);

    protected:
        void clearOnlyShaderProgram();

    protected:
        std::unordered_map<Core::StringAtom, GLint> _uniforms;
        std::function<void()> _setupVertexAttribute;
        Core::StringAtom _name;

        GLuint _vertexShaderId = 0;
        GLuint _fragmentShaderId = 0;
        GLuint _shaderProgramId = 0;

    private:
        [[maybe_unused]] void debugUniform(const Core::StringAtom& name);
    };

} // namespace SW
