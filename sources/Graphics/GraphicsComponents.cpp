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

#include "GraphicsComponents.h"

#include "assimp/scene.h"

namespace SW
{

    GraphicsComponentData::~GraphicsComponentData()
    {
        clear();
    }

    GraphicsComponentData::GraphicsComponentData(GraphicsComponentData&& other) noexcept
    {
        *this = std::move(other);
    }

    GraphicsComponentData& GraphicsComponentData::operator=(GraphicsComponentData&& other) noexcept
    {
        if (this != &other) [[likely]]
        {
            _shader = other._shader;
            _triangleCount = other._triangleCount;
            _vbo = other._vbo;
            _ebo = other._ebo;
            _vao = other._vao;
            _texture = other._texture;

            other._shader = nullptr;
            other._triangleCount = 0;
            other._vbo = 0;
            other._ebo = 0;
            other._vao = 0;
            other._texture = 0;
        }

        return *this;
    }

    void GraphicsComponentData::generate()
    {
        glGenVertexArrays(1, &_vao);
        glGenBuffers(1, &_vbo);
        glGenBuffers(1, &_ebo);
        glGenTextures(1, &_texture);
    }

    void GraphicsComponentData::setVertexBuffer(const std::vector<float>& data, GLenum usage)
    {
        if (_vbo != 0 && _vao != 0) [[likely]]
        {
            glBindVertexArray(_vao);
            glBindBuffer(GL_ARRAY_BUFFER, _vbo);
            glBufferData(GL_ARRAY_BUFFER, sizeof(float) * data.size(), data.data(), GL_STATIC_DRAW);
        }
    }

    void GraphicsComponentData::setIndexBuffer(const std::vector<GLuint>& data, GLenum usage)
    {
        if (_ebo != 0 && _vao != 0) [[likely]]
        {
            glBindVertexArray(_vao);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * data.size(), data.data(),
                         GL_STATIC_DRAW);
            _triangleCount = data.size();
        }
    }

    void GraphicsComponentData::setTexture(const unsigned char* data, uint32_t width,
                                           uint32_t height, GLuint channels)
    {
        if (data && _ebo != 0 && _vao != 0 && _texture != 0) [[likely]]
        {
            glBindVertexArray(_vao);

            glBindTexture(GL_TEXTURE_2D, _texture);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            glTexImage2D(GL_TEXTURE_2D, 0, channels, width, height, 0, channels, GL_UNSIGNED_BYTE,
                         data);
            glGenerateMipmap(GL_TEXTURE_2D);

            glBindTexture(GL_TEXTURE_2D, 0);
        }
    }

    void GraphicsComponentData::setShaderProgram(ShaderProgram* sp,
                                                 bool ignoreVertexAttribSetup /* = false*/)
    {
        _shader = sp;

        if (!ignoreVertexAttribSetup)
        {
            glBindVertexArray(_vao);
            glBindBuffer(GL_ARRAY_BUFFER, _vbo);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
            _shader->setupVertexAttribute();
        }
    }

    void GraphicsComponentData::setMesh(const aiMesh* mesh, bool isAppendNormals /* = false*/,
                                        bool isAppendUV /* = false*/)
    {
        if (!mesh) [[unlikely]]
        {
            globalLog.errorLog("Impossible to set mesh. Mesh object is NULL.");
            return;
        }

        std::vector<float> vertices;
        std::vector<GLuint> indices;

        for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
        {
            const aiFace& face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; ++j)
            {
                indices.push_back(face.mIndices[j]);
            }
        }

        for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
        {
            const aiVector3D v = mesh->mVertices[i];
            vertices.push_back(v.x);
            vertices.push_back(v.y);
            vertices.push_back(v.z);

            if (isAppendNormals)
            {
                const aiVector3D n = mesh->mNormals[i];
                vertices.push_back(n.x);
                vertices.push_back(n.y);
                vertices.push_back(n.z);
            }

            if (isAppendUV)
            {
                if (mesh->HasTextureCoords(0))
                {
                    const auto n = mesh->mTextureCoords[0][i];
                    vertices.push_back(n.x);
                    vertices.push_back(n.y);
                }
                else
                {
                    vertices.push_back(0);
                    vertices.push_back(0);
                }
            }
        }

        setVertexBuffer(vertices);
        setIndexBuffer(indices);

        if (!mesh->mName.Empty()) [[likely]]
        {
            globalLog.debugLog("Mesh '{}' was loaded to the program object."_f
                               << mesh->mName.C_Str());
        }
    }

    void GraphicsComponentData::clear()
    {
        glDeleteTextures(1, &_texture);
        glDeleteBuffers(1, &_ebo);
        glDeleteBuffers(1, &_vbo);
        glDeleteVertexArrays(1, &_vao);
        _shader = nullptr;
        _triangleCount = 0;
    }

    void GraphicsComponentData::directDraw() noexcept
    {
        if (!isValid()) [[unlikely]]
        {
            Assert("Can't draw graphic component. It wasn't configured.");
            return;
        }

        for (auto [val, mod] : _drawModifiers)
        {
            if (mod.cast() == Modifier::Enable)
            {
                glEnable(static_cast<GLenum>(val));
            }
            else if (mod.cast() == Modifier::Disable)
            {
                glDisable(static_cast<GLenum>(val));
            }
        }

        _shader->use();
        glBindVertexArray(_vao);
        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, _texture);

        applyUniforms();

        glDrawElements(GL_TRIANGLES, _triangleCount, GL_UNSIGNED_INT, 0);

        for (auto [val, mod] : _drawModifiers)
        {
            if (mod.cast() == Modifier::Disable)
            {
                glEnable(static_cast<GLenum>(val));
            }
            else if (mod.cast() == Modifier::Enable)
            {
                glDisable(static_cast<GLenum>(val));
            }
        }
    }

    nlohmann::json GraphicsComponentData::toJson() const
    {
        nlohmann::json json;
        json["modifiers"] = nlohmann::json::array();
        for (auto [val, mod] : _drawModifiers)
        {
            nlohmann::json modifier;
            modifier["value"] = ToString(val);
            modifier["modifier"] = mod.toStr();
            json["modifiers"].push_back(std::move(modifier));
        }
        return json;
    }

    void GraphicsComponentData::fromJson(const nlohmann::json& json, bool isIgnoreChildren)
    {
        if (json.contains("modifiers"))
        {
            for (auto&& modifier : json["modifiers"])
            {
                const auto val = FromString(modifier.at("value").get<Core::StringAtom>());
                const auto mod = Modifier::fromStr(modifier.at("modifier").get<std::string>());
                if (mod)
                {
                    _drawModifiers.push_back({val, mod.value()});
                }
            }
        }
    }

    Core::StringAtom GraphicsComponentData::ToString(GraphicsComponentData::ModifiedValue v)
    {
        // clang-format off
        if (MV_Blend == v) return "Blend"_atom;
        if (MV_CullFace == v) return "CullFace"_atom;
        if (MV_DepthTest == v) return "DepthTest"_atom;
        if (MV_Dither == v) return "Dither"_atom;
        if (MV_PolygonOffsetFill == v) return "PolygonOffsetFill"_atom;
        if (MV_PolygonOffsetLine == v) return "PolygonOffsetLine"_atom;
        if (MV_PolygonOffsetPoint == v) return "PolygonOffsetPoint"_atom;
        if (MV_PrimitiveRestart == v) return "PrimitiveRestart"_atom;
        if (MV_PrimitiveRestartFixedIndex == v) return "PrimitiveRestartFixedIndex"_atom;
        if (MV_RasterizerDiscard == v) return "RasterizerDiscard"_atom;
        if (MV_SampleAlphaToCoverage == v) return "SampleAlphaToCoverage"_atom;
        if (MV_SampleAlphaToOne == v) return "SampleAlphaToOne"_atom;
        if (MV_SampleCoverage == v) return "SampleCoverage"_atom;
        if (MV_SampleMask == v) return "SampleMask"_atom;
        if (MV_ScissorTest == v) return "ScissorTest"_atom;
        if (MV_StencilTest == v) return "StencilTest"_atom;
        if (MV_Multisample == v) return "Multisample"_atom;
        if (MV_FramebufferSrgb == v) return "FramebufferSrgb"_atom;
        if (MV_ProgramPointSize == v) return "ProgramPointSize"_atom;
        // clang-format on

        return Core::StringAtom("");
    }

    GraphicsComponentData::ModifiedValue GraphicsComponentData::FromString(
        const Core::StringAtom& str)
    {
        // clang-format off
        if (str == "Blend"_atom) return MV_Blend;
        if (str == "CullFace"_atom) return MV_CullFace;
        if (str == "DepthTest"_atom) return MV_DepthTest;
        if (str == "Dither"_atom) return MV_Dither;
        if (str == "PolygonOffsetFill"_atom) return MV_PolygonOffsetFill;
        if (str == "PolygonOffsetLine"_atom) return MV_PolygonOffsetLine;
        if (str == "PolygonOffsetPoint"_atom) return MV_PolygonOffsetPoint;
        if (str == "PrimitiveRestart"_atom) return MV_PrimitiveRestart;
        if (str == "PrimitiveRestartFixedIndex"_atom) return MV_PrimitiveRestartFixedIndex;
        if (str == "RasterizerDiscard"_atom) return MV_RasterizerDiscard;
        if (str == "SampleAlphaToCoverage"_atom) return MV_SampleAlphaToCoverage;
        if (str == "SampleAlphaToOne"_atom) return MV_SampleAlphaToOne;
        if (str == "SampleCoverage"_atom) return MV_SampleCoverage;
        if (str == "SampleMask"_atom) return MV_SampleMask;
        if (str == "ScissorTest"_atom) return MV_ScissorTest;
        if (str == "StencilTest"_atom) return MV_StencilTest;
        if (str == "Multisample"_atom) return MV_Multisample;
        if (str == "FramebufferSrgb"_atom) return MV_FramebufferSrgb;
        if (str == "ProgramPointSize"_atom) return MV_ProgramPointSize;
        // clang-format on

        return ModifiedValue::MV_None;
    }

    const std::vector<Core::StringAtom>& GraphicsComponentData::ModifiedValueAsVector()
    {
        static const std::vector<Core::StringAtom> out = {
            "Blend"_atom,
            "CullFace"_atom,
            "DepthTest"_atom,
            "Dither"_atom,
            "PolygonOffsetFill"_atom,
            "PolygonOffsetLine"_atom,
            "PolygonOffsetPoint"_atom,
            "PrimitiveRestart"_atom,
            "PrimitiveRestartFixedIndex"_atom,
            "RasterizerDiscard"_atom,
            "SampleAlphaToCoverage"_atom,
            "SampleAlphaToOne"_atom,
            "SampleCoverage"_atom,
            "SampleMask"_atom,
            "ScissorTest"_atom,
            "StencilTest"_atom,
            "Multisample"_atom,
            "FramebufferSrgb"_atom,
            "ProgramPointSize"_atom,
        };
        return out;
    }
    const std::vector<Core::StringAtom>& GraphicsComponentData::ModifierAsVector()
    {
        static const std::vector<Core::StringAtom> out = {
            "Enable",
            "Disable",
        };
        return out;
    }
} // namespace SW