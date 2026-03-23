/*
 * MIT License
 *
 * Copyright (c) 2018-2026 Valerii Koniushenko
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

#include "GraphicsComponents.h"

#include "GameplaySystem/ECS/BaseComponent.h"
#include "assimp/scene.h"

namespace Core
{
    GraphicsComponentData::~GraphicsComponentData()
    {
        privateClear();
    }

    GraphicsComponentData::GraphicsComponentData(GraphicsComponentData&& other) noexcept
        : _drawModifiers({}),
          _shader(other._shader),
          _triangleCount(other._triangleCount),
          _vbo(other._vbo),
          _ebo(other._ebo),
          _vao(other._vao),
          _texture(other._texture)
    {
        other._shader = nullptr;
        other._triangleCount = 0;
        other._vbo = 0;
        other._ebo = 0;
        other._vao = 0;
        other._texture = 0;
    }

    GraphicsComponentData& GraphicsComponentData::operator=(GraphicsComponentData&& other) noexcept
    {
        if (this == &other) [[unlikely]]
        {
            return *this;
        }

        GraphicsComponentData tmp(std::move(other));
        swap(*this, tmp);
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
        if (Verify(_vbo != 0 && _vao != 0)) [[likely]]
        {
            glBindVertexArray(_vao);
            glBindBuffer(GL_ARRAY_BUFFER, _vbo);
            glBufferData(GL_ARRAY_BUFFER, sizeof(float) * data.size(), data.data(), GL_STATIC_DRAW);
        }
    }

    void GraphicsComponentData::setIndexBuffer(const std::vector<GLuint>& data, GLenum usage)
    {
        if (Verify(_ebo != 0 && _vao != 0)) [[likely]]
        {
            glBindVertexArray(_vao);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * data.size(), data.data(),
                         GL_STATIC_DRAW);
            _triangleCount = static_cast<uint32_t>(data.size());
        }
    }

    void GraphicsComponentData::setTexture2D(const unsigned char* data, uint32_t width,
                                             uint32_t height, int channelsCount)
    {
        if (Verify(data && _ebo != 0 && _vao != 0 && _texture != 0)) [[likely]]
        {
            if (!Verify(channelsCount >= 3 && channelsCount <= 4, "Impossible count of channels"))
            {
                return;
            }

            glBindVertexArray(_vao);

            glBindTexture(GL_TEXTURE_2D, _texture);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            const GLenum format = (channelsCount == 4) ? GL_RGBA : GL_RGB;
            const GLenum internalFormat = (channelsCount == 4) ? GL_RGBA8 : GL_RGB8;

            glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format,
                         GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            glBindTexture(GL_TEXTURE_2D, 0);
        }
    }

    void GraphicsComponentData::setShader(ShaderProgram* sp,
                                          bool ignoreVertexAttribSetup /* = false*/)
    {
        _shader = sp;

        if (!ignoreVertexAttribSetup)
        {
            Assert(_vao != 0);
            glBindVertexArray(_vao);
            glBindBuffer(GL_ARRAY_BUFFER, _vbo);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
            _shader->setupVertexAttribute();
        }
    }

    void GraphicsComponentData::setMesh(const aiMesh* mesh, bool isAppendNormals /* = false*/,
                                        bool isAppendUV /* = false*/, float scale /* = 1.f*/)
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
            const aiVector3D v = mesh->mVertices[i] * scale;
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

        // if (!mesh->mName.Empty()) [[likely]]
        // {
        //     globalLog.debugLog("Mesh '{}' was loaded to the world."_f
        //                        << mesh->mName.C_Str());
        // }
    }

    void GraphicsComponentData::clear()
    {
        privateClear();
    }

    void GraphicsComponentData::directDraw(GLenum bindTextureType, GLenum textureIndex) noexcept
    {
        if (!isValid()) [[unlikely]]
        {
            Assert("Can't draw graphic component. It wasn't configured.");
            return;
        }

        for (auto [val, mod] : _drawModifiers)
        {
            if (mod == Modifier::Enable)
            {
                glEnable(static_cast<GLenum>(val));
            }
            else if (mod == Modifier::Disable)
            {
                glDisable(static_cast<GLenum>(val));
            }
        }

        _shader->use();
        glBindVertexArray(_vao);
        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
        glActiveTexture(GL_TEXTURE0 + textureIndex);
        glBindTexture(bindTextureType, _texture);

        applyUniforms();

        glDrawElements(GL_TRIANGLES, _triangleCount, GL_UNSIGNED_INT, nullptr);

        for (auto [val, mod] : _drawModifiers)
        {
            if (mod == Modifier::Disable)
            {
                glEnable(static_cast<GLenum>(val));
            }
            else if (mod == Modifier::Enable)
            {
                glDisable(static_cast<GLenum>(val));
            }
        }
    }

    void GraphicsComponentData::setDrawModifiers(std::vector<ModifierParam>&& values)
    {
#if defined(DEBUG)
        std::map<GLenum, int> map;

        for (auto& [value, modifier] : values)
        {
            map[static_cast<GLenum>(value)]++;
            Assert(map[static_cast<GLenum>(value)] == 1, "The same modifier was added twice.");
        }
#endif

        _drawModifiers = std::move(values);
    }

    void GraphicsComponentData::setDrawModifiers(const std::vector<ModifierParam>& values)
    {
#if defined(DEBUG)
        std::map<GLenum, int> map;

        for (const auto& [value, modifier] : values)
        {
            map[static_cast<GLenum>(value)]++;
            Assert(map[static_cast<GLenum>(value)] == 1, "The same modifier was added twice.");
        }
#endif

        _drawModifiers = values;
    }

    void GraphicsComponentData::addDrawModifiers(ModifiedValue value, Modifier mod)
    {
        if (getDrawModifier(value) != Modifier::None)
        {
            return;
        }

        _drawModifiers.emplace_back(value, mod);
    }

    GraphicsComponentData::Modifier GraphicsComponentData::getDrawModifier(ModifiedValue value)
    {
        const auto it = std::ranges::find_if(_drawModifiers,
                                             [value](auto pair) { return pair.value == value; });

        if (it != _drawModifiers.end())
        {
            return it->modifier;
        }

        return Modifier::None;
    }

    void GraphicsComponentData::ioFieldsUpdate(DataStream& out)
    {
        auto stream = out.dedicatedNesting("GraphicsComponentData");

        stream.array(
            "modifiers",
            [this](DataStream& out, std::size_t size)
            {
                if (out.getMode() == DataStream::Mode::Output)
                {
                    for (auto [val, mod] : _drawModifiers)
                    {
                        nlohmann::json modifier;
                        modifier["value"] = R<decltype(val)>::ToString(val);
                        modifier["modifier"] = R<decltype(mod)>::ToString(mod);
                        out.getRaw().push_back(std::move(modifier));
                    }
                }
                else
                {
                    for (auto& modifier : out.getRaw())
                    {
                        if (!modifier.contains("value") || !modifier.contains("modifier"))
                        {
                            continue;
                        }

                        auto value = R<GraphicsComponentData::ModifiedValue>::FromString(
                            modifier["value"].get<std::string_view>());
                        auto param = R<GraphicsComponentData::Modifier>::FromString(
                            modifier["modifier"].get<std::string_view>());

                        if (!value || !param)
                        {
                            globalLog.errorLog(
                                "Was got invalid GraphicsComponentData ModifiedValue: '{}' or Modifier: '{}'"_f
                                << modifier["value"].get<std::string>()
                                << modifier["modifier"].get<std::string>());
                            continue;
                        }
                        _drawModifiers.emplace_back(value.value(), param.value());
                    }
                }
            });
    }

    StringAtom GraphicsComponentData::getCacheHash() const
    {
        return "GraphicsComponentData"_atom;
    }

    void GraphicsComponentData::privateClear()
    {
        glDeleteTextures(1, &_texture);
        glDeleteBuffers(1, &_ebo);
        glDeleteBuffers(1, &_vbo);
        glDeleteVertexArrays(1, &_vao);
        _shader = nullptr;
        _triangleCount = 0;
    }

} // namespace Core
