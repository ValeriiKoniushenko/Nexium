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

// clang-format off
#include "glad/glad.h"
#include "GLFW/glfw3.h"
// clang-format on

#include "BetterEnums/enum.h"
#include "Utils/Functions.h"

#include <filesystem>

namespace SW
{
    // clang-format off
    BETTER_ENUM(ShaderType, GLenum,
        None = 0,
        Fragment = GL_FRAGMENT_SHADER,
        Vertex = GL_VERTEX_SHADER,
        Geometry = GL_GEOMETRY_SHADER
    );
    // clang-format on

    template<ShaderType shaderType>
    class Shader final
    {
    public:
        Shader() = default;
        ~Shader() = default;

        explicit Shader(const std::filesystem::path& path) { createFromFile(path); }

        Shader(Shader&& other) noexcept { *this = std::move(other); }
        Shader& operator=(Shader&& other) noexcept
        {
            _data = other._data;
            other._data = 0;
            return *this;
        }

        [[nodiscard]] bool isEmpty() const noexcept { return _data == 0; }
        [[nodiscard]] ShaderType getShaderType() const noexcept { return shaderType; }
        [[nodiscard]] GLuint data() noexcept { return _data; }

        void clear()
        {
            if (_data != 0)
            {
                glDeleteShader(_data);
                _data = 0;
            }
        }

        void createFromFile(const std::filesystem::path& path)
        {
            create();
            loadFromFile(path);
            validate();
            compile();
        }

    private:
        void loadFromFile(const std::filesystem::path& path)
        {
            const auto sources = Utils::GetTextFileContentAs<std::string>(path);
            if (sources.empty())
            {
                return;
            }
            const auto* raw = sources.data();
            glShaderSource(_data, 1, &raw, nullptr);
        }

        void create()
        {
            clear();

            _data = glCreateShader(static_cast<GLenum>(shaderType));

            if (isEmpty())
            {
                throw std::runtime_error("Can't create gl shader.");
            }
        }

        void validate() {}

        void compile()
        {
            glCompileShader(_data);
            requireNoCompileErrors();
        }

        void requireNoCompileErrors()
        {
            constexpr auto size = 512;
            int success = 0;
            char infoLog[size];
            glGetShaderiv(_data, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(_data, size, nullptr, infoLog);
                std::string msg = "Shader compilation error: ";
                msg += infoLog;
                throw std::runtime_error(msg);
            }
        }

    private:
        GLuint _data = 0;
    };

    using FragmentShader = Shader<ShaderType::Fragment>;
    using VertexShader = Shader<ShaderType::Vertex>;
    using GeometryShader = Shader<ShaderType::Geometry>;

} // namespace SW
