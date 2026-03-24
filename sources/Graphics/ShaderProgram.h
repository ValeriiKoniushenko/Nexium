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

#include "Core/Color.h"
#include "Misc/BaseLog.h"
#include "OpenGL.h"
#include "glm/fwd.hpp"
#include "glm/gtc/type_ptr.hpp"
namespace Core
{
    class ShaderProgramMeta;

    struct ShaderVariable
    {
        struct Hasher
        {
            std::size_t operator()(const ShaderVariable& obj) const { return obj.name.makeHash(); }
        };

        StringAtom name;
        GLenum type = 0;

        union
        {
            GLint location = 0;
            GLint offset;
        };

        [[nodiscard]] bool operator==(const ShaderVariable& other) const
        {
            return name == other.name;
        }
    };

    // UBO = UniformBufferObject
    struct ShaderUBO
    {
        struct Hasher
        {
            std::size_t operator()(const ShaderUBO& obj) const { return obj.name.makeHash(); }
        };

        StringAtom name;
        GLuint binding = 0;
        GLuint size = 0;

        std::vector<ShaderVariable> vars;

        [[nodiscard]] bool operator==(const ShaderUBO& other) const { return name == other.name; }
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

        void setName(const StringAtom& name);

        [[nodiscard]] const StringAtom& getName() const noexcept { return _name; }

        [[nodiscard]] GLuint getFragmentShader() noexcept { return _fragmentShaderId; }
        [[nodiscard]] GLuint getVertexShader() noexcept { return _vertexShaderId; }

        [[nodiscard]] bool isEmpty() const noexcept { return _shaderProgramId == 0; }

        [[nodiscard]] GLuint getShaderProgramId() const noexcept { return _shaderProgramId; }

        void create(const StringAtom& shaderName);

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

        [[nodiscard]] spdlog::logger* getLogger() const override;

        [[nodiscard]] const char* getPrefix() const override { return "ShaderProgram"; }

        // ================= UNIFORMs =====================
        // All uniform function below MUST be force inlined.
        // So, declare & implement it on the spot.

        // clang-format off
        // Scalars
        void setUniform(const StringAtom& name, GLfloat value) const {  debugUniform(name); glUniform1f(_uniforms.at(name), value); }
        void setUniform(const StringAtom& name, GLint value) const {  debugUniform(name); glUniform1i(_uniforms.at(name), value); }
        void setUniform(const StringAtom& name, GLuint value) const {  debugUniform(name); glUniform1ui(_uniforms.at(name), value); }

        // vec2
        void setUniform(const StringAtom& name, GLfloat x, GLfloat y) const { debugUniform(name); glUniform2f(_uniforms.at(name), x,y); }
        void setUniform(const StringAtom& name, glm::vec2 v) const { setUniform(name, v.x, v.y); }
        void setUniform(const StringAtom& name, GLint x, GLint y) const { debugUniform(name); glUniform2i(_uniforms.at(name), x,y); }
        void setUniform(const StringAtom& name, glm::ivec2 v) const { setUniform(name, v.x, v.y); }
        void setUniform(const StringAtom& name, GLuint x, GLuint y) const { debugUniform(name); glUniform2ui(_uniforms.at(name), x,y); }

        // vec3
        void setUniform(const StringAtom& name, GLfloat x, GLfloat y, GLfloat z) const { debugUniform(name); glUniform3f(_uniforms.at(name), x,y,z); }
        void setUniform(const StringAtom& name, glm::vec3 v) const { setUniform(name, v.x, v.y, v.z); }
        void setUniform(const StringAtom& name, GLint x, GLint y, GLint z) const { debugUniform(name); glUniform3i(_uniforms.at(name), x,y,z); }
        void setUniform(const StringAtom& name, glm::ivec3 v) const { setUniform(name, v.x, v.y, v.z); }
        void setUniform(const StringAtom& name, GLuint x, GLuint y, GLuint z) const { debugUniform(name); glUniform3ui(_uniforms.at(name), x,y,z); }

        // vec4
        void setUniform(const StringAtom& name, GLfloat x, GLfloat y, GLfloat z, GLfloat w) const { debugUniform(name); glUniform4f(_uniforms.at(name), x,y,z,w); }
        void setUniform(const StringAtom& name, GLint x, GLint y, GLint z, GLint w) const { debugUniform(name); glUniform4i(_uniforms.at(name), x,y,z,w); }
        void setUniform(const StringAtom& name, GLuint x, GLuint y, GLuint z, GLuint w) const { debugUniform(name); glUniform4ui(_uniforms.at(name), x,y,z,w); }

        void setUniform(const StringAtom& name, const glm::mat2& value) const { debugUniform(name); glUniformMatrix2fv(_uniforms.at(name), 1, GL_FALSE, glm::value_ptr(value)); }
        void setUniform(const StringAtom& name, const glm::mat3& value) const { debugUniform(name); glUniformMatrix3fv(_uniforms.at(name), 1, GL_FALSE, glm::value_ptr(value)); }
        void setUniform(const StringAtom& name, const glm::mat4& value) const { debugUniform(name); glUniformMatrix4fv(_uniforms.at(name), 1, GL_FALSE, glm::value_ptr(value)); }
        void setUniform(const StringAtom& name, const glm::mat2x3& value) const { debugUniform(name); glUniformMatrix2x3fv(_uniforms.at(name), 1, GL_FALSE, glm::value_ptr(value)); }
        void setUniform(const StringAtom& name, const glm::mat3x2& value) const { debugUniform(name); glUniformMatrix3x2fv(_uniforms.at(name), 1, GL_FALSE, glm::value_ptr(value)); }
        void setUniform(const StringAtom& name, const glm::mat2x4& value) const { debugUniform(name); glUniformMatrix2x4fv(_uniforms.at(name), 1, GL_FALSE, glm::value_ptr(value)); }
        void setUniform(const StringAtom& name, const glm::mat4x2& value) const { debugUniform(name); glUniformMatrix4x2fv(_uniforms.at(name), 1, GL_FALSE, glm::value_ptr(value)); }
        void setUniform(const StringAtom& name, const glm::mat3x4& value) const { debugUniform(name); glUniformMatrix3x4fv(_uniforms.at(name), 1, GL_FALSE, glm::value_ptr(value)); }
        void setUniform(const StringAtom& name, const glm::mat4x3& value) const { debugUniform(name); glUniformMatrix4x3fv(_uniforms.at(name), 1, GL_FALSE, glm::value_ptr(value)); }

        void setUniform(const StringAtom& name, const NormColor4& value) const { debugUniform(name); glUniform4f(_uniforms.at(name), value.r, value.g, value.b, value.a); }
        void setUniform(const StringAtom& name, const NormColor3& value) const { debugUniform(name); glUniform3f(_uniforms.at(name), value.r, value.g, value.b); }
        // clang-format on

        template<typename T>
        void setUniformObject(const StringAtom& name, T&& value) const
        {
#ifdef GRAPHICS_DEBUG
            Assert(name.isStatic(), "Use atomic string. Add _atom to your uniform's str");
            Assert(_ubos.contains(name));
            // Assert(_ubos.at(name) != 0);
#endif
            glUniformBlockBinding(_shaderProgramId, _ubos.at(name), 0);
            glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(T), &value);
        }

        void setDataFromMeta(const ShaderProgramMeta& source);

        void setupVertexAttribute();

        void setVertexAttributeCallback(std::function<void()>&& func);

    protected:
        void clearOnlyShaderProgram();

    protected:
        std::unordered_map<StringAtom, GLint> _uniforms;
        std::unordered_map<StringAtom, GLuint> _ubos;

        std::function<void()> _setupVertexAttribute;
        StringAtom _name;

        GLuint _vertexShaderId = 0;
        GLuint _fragmentShaderId = 0;
        GLuint _shaderProgramId = 0;

    private:
        [[maybe_unused]] void debugUniform(const StringAtom& name) const;
    };
} // namespace Core
