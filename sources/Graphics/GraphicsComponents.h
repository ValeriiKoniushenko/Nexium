/*
 * MIT License
 *
 * Copyright (c) 2018-2027 Valerii Koniushenko
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

#include "ResourceManagement/DataStream.h"
#include "ResourceManagement/JsonAdapter.h"
#include "ShaderProgram.h"
#include "assimp/mesh.h"

namespace Core
{

    // ╔════════════════════════════════════════════════════════╗
    // ║                                                        ║
    // ║     ╔╗ ┌─┐┌─┐┌─┐╔═╗┬─┐┌─┐┌─┐┬ ┬┬┌─┐┌─┐╔╦╗┌─┐┌┬┐┌─┐     ║
    // ║     ╠╩╗├─┤└─┐├┤ ║ ╦├┬┘├─┤├─┘├─┤││  └─┐ ║║├─┤ │ ├─┤     ║
    // ║     ╚═╝┴ ┴└─┘└─┘╚═╝┴└─┴ ┴┴  ┴ ┴┴└─┘└─┘═╩╝┴ ┴ ┴ ┴ ┴     ║
    // ║                                                        ║
    // ╚════════════════════════════════════════════════════════╝
    CLASS();
    class BaseGraphicsData : public IDataIO
    {
        R_FRIEND(BaseGraphicsData);

    public:
        ENUM_CLASS();
        enum class Modifier : std::uint8_t
        {
            None,
            Enable,
            Disable,
        };

        ENUM_CLASS();
        enum class ModifiedValue : GLenum
        {
            None = 0,
            Blend = GL_BLEND,
            CullFace = GL_CULL_FACE,
            DepthTest = GL_DEPTH_TEST,
            Dither = GL_DITHER,
            PolygonOffsetFill = GL_POLYGON_OFFSET_FILL,
            PolygonOffsetLine = GL_POLYGON_OFFSET_LINE,
            PolygonOffsetPoint = GL_POLYGON_OFFSET_POINT,
            PrimitiveRestart = GL_PRIMITIVE_RESTART,
            PrimitiveRestartFixedIndex = GL_PRIMITIVE_RESTART_FIXED_INDEX,
            RasterizerDiscard = GL_RASTERIZER_DISCARD,
            SampleAlphaToCoverage = GL_SAMPLE_ALPHA_TO_COVERAGE,
            SampleAlphaToOne = GL_SAMPLE_ALPHA_TO_ONE,
            SampleCoverage = GL_SAMPLE_COVERAGE,
            SampleMask = GL_SAMPLE_MASK,
            ScissorTest = GL_SCISSOR_TEST,
            StencilTest = GL_STENCIL_TEST,
            Multisample = GL_MULTISAMPLE,
            FramebufferSrgb = GL_FRAMEBUFFER_SRGB,
            ProgramPointSize = GL_PROGRAM_POINT_SIZE,
        };

        struct ModifierParam
        {
            ModifiedValue value = ModifiedValue::None;
            Modifier modifier = Modifier::None;
        };

    public:
        BaseGraphicsData() = default;
        ~BaseGraphicsData() override;
        BaseGraphicsData(const BaseGraphicsData& other) = default;
        BaseGraphicsData(BaseGraphicsData&& other) noexcept;
        BaseGraphicsData& operator=(const BaseGraphicsData& other) = default;
        BaseGraphicsData& operator=(BaseGraphicsData&& other) noexcept;

        [[nodiscard]] virtual nlohmann::json serialize() const;
        virtual void deserialize(RResourceStream<RJsonResourceStream>& stream);

        [[nodiscard]] ShaderProgram* getShader() noexcept { return _shader; }

        [[nodiscard]] GLuint getVboId() noexcept { return _vbo; }
        [[nodiscard]] GLuint getEboId() noexcept { return _ebo; }
        [[nodiscard]] GLuint getVaoId() noexcept { return _vao; }

        void bindVAO() const noexcept
        {
            Assert(_vao != 0);
            glBindVertexArray(_vao);
        }

        void unbindVao() const noexcept { glBindVertexArray(0); }

        void bindVBO() const noexcept
        {
            Assert(_vbo != 0);
            glBindBuffer(GL_ARRAY_BUFFER, _vbo);
        }

        void bindEBO() const noexcept
        {
            Assert(_ebo != 0);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
        }

        virtual void bindAllBuffers() const noexcept
        {
            bindVAO();
            bindVBO();
            bindEBO();
        }

        void setVertexBuffer(const std::vector<float>& data, GLenum usage = GL_STATIC_DRAW);

        void setIndexBuffer(const std::vector<GLuint>& data, GLenum usage = GL_STATIC_DRAW);

        void setShader(ShaderProgram* sp, bool ignoreVertexAttribSetup = false);

        virtual void generate();

        virtual void clear();
        [[nodiscard]] uint32_t getTriangleCount() const noexcept { return _triangleCount; }
        [[nodiscard]] virtual bool isValid() const noexcept
        {
            return _vbo != 0 && _ebo != 0 && _vao != 0 && _shader != nullptr;
        }

        /**
         * direct draw if was set up vertex, index and optionally texture buffers.
         * @details this function from the family of 'low-abstract' functionality.
         * It can give you the ability for fast setup & draw to the scene but without huge
         * or at least some small optimizations. Just hard-drawn in the old-school opengl
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
         * InterleavedGraphicsData x;
         * x.generate();
         * x.setVertexBuffer(vertices);
         * x.setIndexBuffer(indices);
         * x.setShader(some_compiled_shader_program);
         *
         * while(...)
         * {
         *     ...
         *     x.directDraw(GL_TRIANGLES);
         *     ...
         * }
         * @endcode
         */
        virtual void directDraw(GLenum drawMode = GL_TRIANGLES,
                                GLenum bindTextureType = GL_TEXTURE_2D, GLenum textureIndex = 0);

        // ====================== Draw modifiers ====================

        [[nodiscard]] const std::vector<ModifierParam>& getDrawModifiers() const noexcept
        {
            return _drawModifiers;
        }

        void clearDrawModifiers() { _drawModifiers.clear(); }

        void setDrawModifiers(std::vector<ModifierParam>&& values);

        void setDrawModifiers(const std::vector<ModifierParam>& values);

        void addDrawModifiers(ModifiedValue value, Modifier mod);

        [[nodiscard]] Modifier getDrawModifier(ModifiedValue value);

        // ====================== Misc ====================
        friend void swap(BaseGraphicsData& a, BaseGraphicsData& b) noexcept
        {
            std::swap(a._shader, b._shader);
            std::swap(a._drawModifiers, b._drawModifiers);
            std::swap(a._triangleCount, b._triangleCount);
            std::swap(a._vbo, b._vbo);
            std::swap(a._ebo, b._ebo);
            std::swap(a._vao, b._vao);
        }

        [[nodiscard]] StringAtom getCacheHash() const override;

    protected:
        virtual void onBindBuffers(GLenum bindTextureType, GLenum textureIndex) {}

    private:
        void privateClear();

    private:
        ShaderProgram* _shader = nullptr;

        // To improve cache-line readability, we use vector.
        // But all values ModifiedValue should be unique.
        FIELD();
        std::vector<ModifierParam> _drawModifiers;

        GLsizei _triangleCount = 0;
        GLuint _vbo = 0;
        GLuint _ebo = 0;
        GLuint _vao = 0;
    };

    // ╔══════════════════════════════════════════════════════════════════════════════╗
    // ║                                                                              ║
    // ║     ╔╗ ┌─┐┌─┐┌─┐╔╦╗┌─┐─┐ ┬┌┬┐┬ ┬┬─┐┌─┐╔═╗┬─┐┌─┐┌─┐┬ ┬┬┌─┐┌─┐╔╦╗┌─┐┌┬┐┌─┐     ║
    // ║     ╠╩╗├─┤└─┐├┤  ║ ├┤ ┌┴┬┘ │ │ │├┬┘├┤ ║ ╦├┬┘├─┤├─┘├─┤││  └─┐ ║║├─┤ │ ├─┤     ║
    // ║     ╚═╝┴ ┴└─┘└─┘ ╩ └─┘┴ └─ ┴ └─┘┴└─└─┘╚═╝┴└─┴ ┴┴  ┴ ┴┴└─┘└─┘═╩╝┴ ┴ ┴ ┴ ┴     ║
    // ║                                                                              ║
    // ╚══════════════════════════════════════════════════════════════════════════════╝
    CLASS();
    class BaseTextureGraphicsData : public BaseGraphicsData
    {
        R_FRIEND_DECL(Core::BaseTextureGraphicsData, Core::BaseGraphicsData);

    public:
        BaseTextureGraphicsData() = default;
        ~BaseTextureGraphicsData() override;

        BaseTextureGraphicsData(const BaseTextureGraphicsData& other) = default;
        BaseTextureGraphicsData(BaseTextureGraphicsData&& other) noexcept;
        BaseTextureGraphicsData& operator=(const BaseTextureGraphicsData& other) = default;
        BaseTextureGraphicsData& operator=(BaseTextureGraphicsData&& other) noexcept;

        void generate() override;
        void clear() override;
        [[nodiscard]] bool isValid() const noexcept override
        {
            return BaseGraphicsData::isValid() && _texture != 0;
        }

        [[nodiscard]] GLuint getTextureId() noexcept { return _texture; }
        void bindTexture(GLenum type = GL_TEXTURE_2D) const noexcept
        {
            Assert(_texture != 0);
            glBindTexture(type, _texture);
        }

        void setTexture2D(const unsigned char* data, uint32_t width, uint32_t height,
                          int channelsCount);

        // =================== MISC ===================
        friend void swap(BaseTextureGraphicsData& a, BaseTextureGraphicsData& b) noexcept
        {
            swap(static_cast<BaseGraphicsData&>(a), static_cast<BaseGraphicsData&>(b));
            std::swap(a._texture, b._texture);
        }
        [[nodiscard]] StringAtom getCacheHash() const override;

    protected:
        void onBindBuffers(GLenum bindTextureType, GLenum textureIndex) override;

    private:
        void privateClear();

    private:
        GLuint _texture = 0;
    };

    // ╔════════════════════════════════════════════════════════════════════════════╗
    // ║                                                                            ║
    // ║     ╦┌┐┌┌┬┐┌─┐┬─┐┬  ┌─┐┌─┐┬  ┬┌─┐┌┬┐╔═╗┬─┐┌─┐┌─┐┬ ┬┬┌─┐┌─┐╔╦╗┌─┐┌┬┐┌─┐     ║
    // ║     ║│││ │ ├┤ ├┬┘│  ├┤ ├─┤└┐┌┘├┤  ││║ ╦├┬┘├─┤├─┘├─┤││  └─┐ ║║├─┤ │ ├─┤     ║
    // ║     ╩┘└┘ ┴ └─┘┴└─┴─┘└─┘┴ ┴ └┘ └─┘─┴┘╚═╝┴└─┴ ┴┴  ┴ ┴┴└─┘└─┘═╩╝┴ ┴ ┴ ┴ ┴     ║
    // ║                                                                            ║
    // ╚════════════════════════════════════════════════════════════════════════════╝
    CLASS();
    class InterleavedGraphicsData : public BaseTextureGraphicsData
    {
        R_FRIEND_DECL(Core::InterleavedGraphicsData, Core::BaseGraphicsData);

    public:
        InterleavedGraphicsData() = default;
        ~InterleavedGraphicsData() override = default;

        InterleavedGraphicsData(const InterleavedGraphicsData& other) = default;
        InterleavedGraphicsData(InterleavedGraphicsData&& other) noexcept;
        InterleavedGraphicsData& operator=(const InterleavedGraphicsData& other) = default;
        InterleavedGraphicsData& operator=(InterleavedGraphicsData&& other) noexcept;

        /**
         * loads & constructs from aiMesh GPU data.
         * @param mesh from Assimp::Importer
         * @param isAppendNormals if it true - than it will append to every vertex data 'normals'
         * @param isAppendUV if it true - than it will append to every vertex data 'UV'
         * @param scale mesh scaler
         *
         * Example of one vertex data:
         * pos.x, pos.y, pos.z,  [normal.x, normal.y, normal.z]  [cv.x, cv.y]
         */
        void setMesh(const aiMesh* mesh, bool isAppendNormals = false, bool isAppendUV = false,
                     float scale = 1.f);

        // =================== MISC ===================
        friend void swap(InterleavedGraphicsData& a, InterleavedGraphicsData& b) noexcept
        {
            swap(static_cast<BaseTextureGraphicsData&>(a),
                 static_cast<BaseTextureGraphicsData&>(b));
        }
        [[nodiscard]] StringAtom getCacheHash() const override;
    };

    // ╔═════════════════════════════════════════════════════════════════════════════════╗
    // ║                                                                                 ║
    // ║     ╔═╗┌─┐┌─┐┌─┐┬─┐╔╦╗┌─┐─┐ ┬┌┬┐┬ ┬┬─┐┌─┐╔═╗┬─┐┌─┐┌─┐┬ ┬┬┌─┐┌─┐╔╦╗┌─┐┌┬┐┌─┐     ║
    // ║     ╚═╗├┤ ├─┘├─┤├┬┘ ║ ├┤ ┌┴┬┘ │ │ │├┬┘├┤ ║ ╦├┬┘├─┤├─┘├─┤││  └─┐ ║║├─┤ │ ├─┤     ║
    // ║     ╚═╝└─┘┴  ┴ ┴┴└─ ╩ └─┘┴ └─ ┴ └─┘┴└─└─┘╚═╝┴└─┴ ┴┴  ┴ ┴┴└─┘└─┘═╩╝┴ ┴ ┴ ┴ ┴     ║
    // ║                                                                                 ║
    // ╚═════════════════════════════════════════════════════════════════════════════════╝
    CLASS();
    class SeparTextureGraphicsData : public BaseTextureGraphicsData
    {
        R_FRIEND_DECL(Core::SeparTextureGraphicsData, Core::BaseTextureGraphicsData);

    public:
        SeparTextureGraphicsData() = default;
        ~SeparTextureGraphicsData() override;

        SeparTextureGraphicsData(const SeparTextureGraphicsData& other) = default;
        SeparTextureGraphicsData(SeparTextureGraphicsData&& other) noexcept;
        SeparTextureGraphicsData& operator=(const SeparTextureGraphicsData& other) = default;
        SeparTextureGraphicsData& operator=(SeparTextureGraphicsData&& other) noexcept;

        void generate() override;
        void clear() override;
        [[nodiscard]] bool isValid() const noexcept override;

        void bindTextureVBO() const noexcept
        {
            Assert(_textureVbo != 0);
            glBindBuffer(GL_ARRAY_BUFFER, _textureVbo);
        }
        void unbindTextureVBO() const noexcept { glBindBuffer(GL_ARRAY_BUFFER, 0); }
        [[nodiscard]] GLuint getTextureVboId() noexcept { return _textureVbo; }

        void setTextureVertexBuffer(const std::vector<float>& data, GLenum usage = GL_STATIC_DRAW);

        void bindAllBuffers() const noexcept override
        {
            BaseGraphicsData::bindAllBuffers();
            bindTextureVBO();
        }

        // =================== MISC ===================
        friend void swap(SeparTextureGraphicsData& a, SeparTextureGraphicsData& b) noexcept
        {
            swap(static_cast<BaseTextureGraphicsData&>(a),
                 static_cast<BaseTextureGraphicsData&>(b));
            std::swap(a._textureVbo, b._textureVbo);
        }

        [[nodiscard]] StringAtom getCacheHash() const override;

    private:
        void privateClear();

    private:
        GLuint _textureVbo = 0;
    };

    // ╔════════════════════════════╗
    // ║                            ║
    // ║     ╔═╗┬  ┌─┐┌┐ ┌─┐┬       ║
    // ║     ║ ╦│  │ │├┴┐├─┤│       ║
    // ║     ╚═╝┴─┘└─┘└─┘┴ ┴┴─┘     ║
    // ║                            ║
    // ╚════════════════════════════╝
    void to_json(nlohmann::json& j, const Core::BaseGraphicsData::ModifierParam& v);
    void from_json(const nlohmann::json& j, Core::BaseGraphicsData::ModifierParam& v);

} // namespace Core

#include "GraphicsComponents.generated.h" // added by the code generator. Better don't move it.
