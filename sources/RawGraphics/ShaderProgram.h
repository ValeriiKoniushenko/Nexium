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
#include "Shader.h"

namespace SW
{

    class ShaderProgram : public PS::BaseLog
    {
    public:
        ~ShaderProgram() override = default;

        // only r-reference
        void setShader(FragmentShader&& shader) { _fragmentShader = std::move(shader); }
        void setShader(VertexShader&& shader) { _vertexShader = std::move(shader); }

        [[nodiscard]] const FragmentShader& getFragmentShader() const noexcept
        {
            return _fragmentShader;
        }
        [[nodiscard]] const VertexShader& getVertexShader() const noexcept { return _vertexShader; }

        [[nodiscard]] FragmentShader& getFragmentShader() noexcept { return _fragmentShader; }
        [[nodiscard]] VertexShader& getVertexShader() noexcept { return _vertexShader; }

        [[nodiscard]] spdlog::logger* getLogger() const override
        {
            return RawGraphics::getLogger();
        }

        [[nodiscard]] const char* getPrefix() const override { return "ShaderProgram"; }

        [[nodiscard]] bool isEmpty() const noexcept { return _data == 0; }

        void create(const Core::StringAtom& shaderName);
        void clear();
#ifdef GRAPHICS_DEBUG
        void use() const noexcept
        {
            if (isEmpty())
            {
                criticalThrowingLog("Impossible to use not-created shader program.");
            }
            glUseProgram(_data);
        }
#else
        void use() const noexcept { glUseProgram(_data); }
#endif

    protected:
        void clearOnlyShaderProgram();

    protected:
        FragmentShader _fragmentShader;
        VertexShader _vertexShader;
        Core::StringAtom _name;
        GLuint _data = 0;
    };

} // namespace SW
