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

#include "Core/Enum.h"
#include "ShaderProgram.h"
#include "assimp/mesh.h"

namespace SW
{

    class GraphicsComponentData : public virtual JsonAdapter
    {
    public:
        // clang-format off
        CreateEnum(Modifier, int,
            Enable,
            Disable,
        );
        // clang-format on

        enum ModifiedValue : GLenum
        {
            MV_Blend = GL_BLEND,
            MV_CullFace = GL_CULL_FACE,
            MV_DepthTest = GL_DEPTH_TEST,
            MV_Dither = GL_DITHER,
            MV_PolygonOffsetFill = GL_POLYGON_OFFSET_FILL,
            MV_PolygonOffsetLine = GL_POLYGON_OFFSET_LINE,
            MV_PolygonOffsetPoint = GL_POLYGON_OFFSET_POINT,
            MV_PrimitiveRestart = GL_PRIMITIVE_RESTART,
            MV_PrimitiveRestartFixedIndex = GL_PRIMITIVE_RESTART_FIXED_INDEX,
            MV_RasterizerDiscard = GL_RASTERIZER_DISCARD,
            MV_SampleAlphaToCoverage = GL_SAMPLE_ALPHA_TO_COVERAGE,
            MV_SampleAlphaToOne = GL_SAMPLE_ALPHA_TO_ONE,
            MV_SampleCoverage = GL_SAMPLE_COVERAGE,
            MV_SampleMask = GL_SAMPLE_MASK,
            MV_ScissorTest = GL_SCISSOR_TEST,
            MV_StencilTest = GL_STENCIL_TEST,
            MV_Multisample = GL_MULTISAMPLE,
            MV_FramebufferSrgb = GL_FRAMEBUFFER_SRGB,
            MV_ProgramPointSize = GL_PROGRAM_POINT_SIZE,
            MV_None
        };

        [[nodiscard]] static Core::StringAtom ToString(ModifiedValue v);
        [[nodiscard]] static ModifiedValue FromString(const Core::StringAtom& str);
        [[nodiscard]] static const std::vector<Core::StringAtom>& ModifiedValueAsVector();

    public:
        GraphicsComponentData() = default;
        ~GraphicsComponentData() override;
        GraphicsComponentData(const GraphicsComponentData& other) = delete;
        GraphicsComponentData(GraphicsComponentData&& other) noexcept;
        GraphicsComponentData& operator=(const GraphicsComponentData& other) = delete;
        GraphicsComponentData& operator=(GraphicsComponentData&& other) noexcept;

        void generate();

        void setVertexBuffer(const std::vector<float>& data, GLenum usage = GL_STATIC_DRAW);
        void setIndexBuffer(const std::vector<GLuint>& data, GLenum usage = GL_STATIC_DRAW);
        void setTexture(const unsigned char* data, uint32_t width, uint32_t height,
                        GLuint channels);
        void setShaderProgram(ShaderProgram* sp, bool ignoreVertexAttribSetup = false);

        /**
         * @brief loads & constructs from aiMesh GPU data.
         * @param mesh from Assimp::Importer
         * @param isAppendNormals if it true - than it will append to every vertex data 'normals'
         * @param isAppendUV if it true - than it will append to every vertex data 'UV'
         *
         * Example of one vertex data:
         * pos.x, pos.y, pos.z,  [normal.x, normal.y, normal.z]  [cv.x, cv.y]
         */
        void setMesh(const aiMesh* mesh, bool isAppendNormals = false, bool isAppendUV = false);

        void clear();
        [[nodiscard]] bool isValid() const noexcept
        {
            return _vbo != 0 && _ebo != 0 && _vao != 0 && _shader != 0;
        }

        /**
         * @brief direct draw if was set up vertex, index and optionally texture buffers.
         * @details this function from the family of 'low-abstract' functionality.
         * It can give you ability for fast setup & draw to the scene but without huge
         * or at least some small optimizations. Just hard-draw in the old-school opengl
         * style.
         * Example code(little bit pseudo):
         * @code
         * std::vector<float> vertices = {
         *      0.5f,  0.5f, 0.0f,  // top right
         *      0.5f, -0.5f, 0.0f,  // bottom right
         *     -0.5f, -0.5f, 0.0f,  // bottom left
         *     -0.5f,  0.5f, 0.0f,  // top left
         * };
         *
         * std::vector<float> indices = {
         *      0, 1, 3,  // first Triangle
         *      1, 2, 3   // second Triangle
         * };
         *
         * GraphicsComponentData x;
         * x.generate();
         * x.setVertexBuffer(vertices);
         * x.setIndexBuffer(indices);
         * x.setShaderProgram(some_compiled_shader_program);
         *
         * while(...)
         * {
         *     ...
         *     x.directDraw();
         *     ...
         * }
         * @endcode
         */
        void directDraw() noexcept;

        [[nodiscard]] GLuint getVboId() noexcept { return _vbo; }
        [[nodiscard]] GLuint getEboId() noexcept { return _ebo; }
        [[nodiscard]] GLuint getVaoId() noexcept { return _vao; }
        [[nodiscard]] ShaderProgram* getShaderId() noexcept { return _shader; }

        [[nodiscard]] const std::unordered_map<ModifiedValue, Modifier>& getDrawModifiers() const noexcept
        {
            return _drawModifiers;
        }
        void clearDrawModifiers() { _drawModifiers.clear(); }
        void setDrawModifiers(std::unordered_map<ModifiedValue, Modifier> value)
        {
            _drawModifiers = std::move(value);
        }

        [[nodiscard]] nlohmann::json toJson() const override;
        void fromJson(const nlohmann::json& json, bool isIgnoreChildren) override;

        [[nodiscard]] uint32_t getTriangleCount() const noexcept { return _triangleCount; }

    protected:
        virtual void applyUniforms() {}

    protected:
        std::unordered_map<ModifiedValue, Modifier> _drawModifiers;
        ShaderProgram* _shader = nullptr;
        uint32_t _triangleCount = 0;
        GLuint _vbo = 0;
        GLuint _ebo = 0;
        GLuint _vao = 0;
        GLuint _texture = 0;
    };

} // namespace SW