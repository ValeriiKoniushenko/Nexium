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

namespace Core
{
    R_FRIEND_IMPL(BaseTextureGraphicsData);
    R_FRIEND_IMPL(InterleavedGraphicsData);
    R_FRIEND_IMPL(SeparTextureGraphicsData);

    // ╔════════════════════════════════════════════════════════╗
    // ║                                                        ║
    // ║     ╔╗ ┌─┐┌─┐┌─┐╔═╗┬─┐┌─┐┌─┐┬ ┬┬┌─┐┌─┐╔╦╗┌─┐┌┬┐┌─┐     ║
    // ║     ╠╩╗├─┤└─┐├┤ ║ ╦├┬┘├─┤├─┘├─┤││  └─┐ ║║├─┤ │ ├─┤     ║
    // ║     ╚═╝┴ ┴└─┘└─┘╚═╝┴└─┴ ┴┴  ┴ ┴┴└─┘└─┘═╩╝┴ ┴ ┴ ┴ ┴     ║
    // ║                                                        ║
    // ╚════════════════════════════════════════════════════════╝
    nlohmann::json BaseGraphicsData::serialize() const
    {
        return R<BaseGraphicsData>::Serialize<RJsonResourceStream>(*this).getData();
    }

    void BaseGraphicsData::deserialize(RResourceStream<RJsonResourceStream>& stream)
    {
        R<BaseGraphicsData>::Deserialize(stream, *this);
    }

    void BaseGraphicsData::clear()
    {
        privateClear();
    }

    void BaseGraphicsData::privateClear()
    {
        glDeleteBuffers(1, &_ebo);
        glDeleteBuffers(1, &_vbo);
        glDeleteVertexArrays(1, &_vao);
        _shader = nullptr;
        _triangleCount = 0;
    }

    StringAtom BaseGraphicsData::getCacheHash() const
    {
        return "BaseGraphicsData"_atom;
    }

    BaseGraphicsData::~BaseGraphicsData()
    {
        privateClear();
    }

    BaseGraphicsData::BaseGraphicsData(BaseGraphicsData&& other) noexcept
        : _shader(other._shader),
          _drawModifiers({}),
          _triangleCount(other._triangleCount),
          _vbo(other._vbo),
          _ebo(other._ebo),
          _vao(other._vao)

    {
        other._shader = nullptr;
        other._triangleCount = 0;
        other._vbo = 0;
        other._ebo = 0;
        other._vao = 0;
    }

    BaseGraphicsData& BaseGraphicsData::operator=(BaseGraphicsData&& other) noexcept
    {
        if (this == &other) [[unlikely]]
        {
            return *this;
        }

        BaseGraphicsData tmp(std::move(other));
        swap(*this, tmp);
        return *this;
    }

    void BaseGraphicsData::generate()
    {
        if (Verify(!isValid())) [[likely]]
        {
            glGenVertexArrays(1, &_vao);
            glGenBuffers(1, &_vbo);
            glGenBuffers(1, &_ebo);
        }
    }

    void BaseGraphicsData::setVertexBuffer(const std::vector<float>& data, GLenum usage)
    {
        if (Verify(_vbo != 0 && _vao != 0)) [[likely]]
        {
            bindVAO();
            bindVBO();
            glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(sizeof(float) * data.size()),
                         data.data(), GL_STATIC_DRAW);
        }
    }

    void BaseGraphicsData::setIndexBuffer(const std::vector<GLuint>& data, GLenum usage)
    {
        if (Verify(_ebo != 0 && _vao != 0)) [[likely]]
        {
            bindVAO();
            bindEBO();
            glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                         static_cast<GLsizeiptr>(sizeof(GLuint) * data.size()), data.data(),
                         GL_STATIC_DRAW);
            _triangleCount = static_cast<decltype(_triangleCount)>(data.size());
        }
    }

    void BaseGraphicsData::setShader(ShaderProgram* sp, bool ignoreVertexAttribSetup /* = false*/)
    {
        _shader = sp;

        Assert(_vao != 0);
        if (!ignoreVertexAttribSetup && _vao != 0)
        {
            bindAllBuffers();
            _shader->setupVertexAttribute();
        }
    }

    void BaseGraphicsData::directDraw(GLenum drawMode, GLenum bindTextureType, GLenum textureIndex)
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

        getShader()->use();
        bindAllBuffers();

        onBindBuffers(bindTextureType, textureIndex);

        glDrawElements(drawMode, static_cast<GLsizei>(getTriangleCount()), GL_UNSIGNED_INT,
                       nullptr);

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

    void BaseGraphicsData::setDrawModifiers(std::vector<ModifierParam>&& values)
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

    void BaseGraphicsData::setDrawModifiers(const std::vector<ModifierParam>& values)
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

    void BaseGraphicsData::addDrawModifiers(ModifiedValue value, Modifier mod)
    {
        if (getDrawModifier(value) != Modifier::None)
        {
            return;
        }

        _drawModifiers.emplace_back(value, mod);
    }

    BaseGraphicsData::Modifier BaseGraphicsData::getDrawModifier(ModifiedValue value)
    {
        const auto it = std::ranges::find_if(_drawModifiers,
                                             [value](auto pair) { return pair.value == value; });

        if (it != _drawModifiers.end())
        {
            return it->modifier;
        }

        return Modifier::None;
    }

    // ╔══════════════════════════════════════════════════════════════════════════════╗
    // ║                                                                              ║
    // ║     ╔╗ ┌─┐┌─┐┌─┐╔╦╗┌─┐─┐ ┬┌┬┐┬ ┬┬─┐┌─┐╔═╗┬─┐┌─┐┌─┐┬ ┬┬┌─┐┌─┐╔╦╗┌─┐┌┬┐┌─┐     ║
    // ║     ╠╩╗├─┤└─┐├┤  ║ ├┤ ┌┴┬┘ │ │ │├┬┘├┤ ║ ╦├┬┘├─┤├─┘├─┤││  └─┐ ║║├─┤ │ ├─┤     ║
    // ║     ╚═╝┴ ┴└─┘└─┘ ╩ └─┘┴ └─ ┴ └─┘┴└─└─┘╚═╝┴└─┴ ┴┴  ┴ ┴┴└─┘└─┘═╩╝┴ ┴ ┴ ┴ ┴     ║
    // ║                                                                              ║
    // ╚══════════════════════════════════════════════════════════════════════════════╝

    BaseTextureGraphicsData::~BaseTextureGraphicsData()
    {
        privateClear();
    }

    BaseTextureGraphicsData::BaseTextureGraphicsData(BaseTextureGraphicsData&& other) noexcept
        : BaseGraphicsData(std::move(other)),
          _texture(other._texture)
    {
        other._texture = 0;
    }

    BaseTextureGraphicsData& BaseTextureGraphicsData::operator=(
        BaseTextureGraphicsData&& other) noexcept
    {
        if (this == &other) [[unlikely]]
        {
            return *this;
        }

        BaseTextureGraphicsData tmp(std::move(other));
        swap(*this, tmp);
        return *this;
    }

    void BaseTextureGraphicsData::generate()
    {
        if (Verify(!isValid())) [[likely]]
        {
            BaseGraphicsData::generate();
            glGenTextures(1, &_texture);
        }
    }

    StringAtom BaseTextureGraphicsData::getCacheHash() const
    {
        return "BaseTextureGraphicsData"_atom;
    }

    void BaseTextureGraphicsData::setTexture2D(const unsigned char* data, uint32_t width,
                                               uint32_t height, int channelsCount)
    {
        if (Verify(data && getEboId() != 0 && getVaoId() != 0 && _texture != 0)) [[likely]]
        {
            if (!Verify(channelsCount >= 3 && channelsCount <= 4, "Impossible count of channels"))
            {
                return;
            }

            bindVAO();
            bindTexture();

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            const GLint format = (channelsCount == 4) ? GL_RGBA : GL_RGB;
            const GLint internalFormat = (channelsCount == 4) ? GL_RGBA8 : GL_RGB8;

            glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, static_cast<GLsizei>(width),
                         static_cast<GLsizei>(height), 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            glBindTexture(GL_TEXTURE_2D, 0);
        }
    }

    void BaseTextureGraphicsData::clear()
    {
        BaseGraphicsData::clear();
        privateClear();
    }
    void BaseTextureGraphicsData::onBindBuffers(GLenum bindTextureType, GLenum textureIndex)
    {
        // glActiveTexture(GL_TEXTURE0 + textureIndex);
        // bindTexture();
    }

    void BaseTextureGraphicsData::privateClear()
    {
        glDeleteTextures(1, &_texture);
    }

    // ╔════════════════════════════════════════════════════════════════════════════╗
    // ║                                                                            ║
    // ║     ╦┌┐┌┌┬┐┌─┐┬─┐┬  ┌─┐┌─┐┬  ┬┌─┐┌┬┐╔═╗┬─┐┌─┐┌─┐┬ ┬┬┌─┐┌─┐╔╦╗┌─┐┌┬┐┌─┐     ║
    // ║     ║│││ │ ├┤ ├┬┘│  ├┤ ├─┤└┐┌┘├┤  ││║ ╦├┬┘├─┤├─┘├─┤││  └─┐ ║║├─┤ │ ├─┤     ║
    // ║     ╩┘└┘ ┴ └─┘┴└─┴─┘└─┘┴ ┴ └┘ └─┘─┴┘╚═╝┴└─┴ ┴┴  ┴ ┴┴└─┘└─┘═╩╝┴ ┴ ┴ ┴ ┴     ║
    // ║                                                                            ║
    // ╚════════════════════════════════════════════════════════════════════════════╝
    void InterleavedGraphicsData::setMesh(const aiMesh* mesh, bool isAppendNormals /* = false*/,
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

    StringAtom InterleavedGraphicsData::getCacheHash() const
    {
        return "InterleavedGraphicsData"_atom;
    }

    // ╔═════════════════════════════════════════════════════════════════════════════════╗
    // ║                                                                                 ║
    // ║     ╔═╗┌─┐┌─┐┌─┐┬─┐╔╦╗┌─┐─┐ ┬┌┬┐┬ ┬┬─┐┌─┐╔═╗┬─┐┌─┐┌─┐┬ ┬┬┌─┐┌─┐╔╦╗┌─┐┌┬┐┌─┐     ║
    // ║     ╚═╗├┤ ├─┘├─┤├┬┘ ║ ├┤ ┌┴┬┘ │ │ │├┬┘├┤ ║ ╦├┬┘├─┤├─┘├─┤││  └─┐ ║║├─┤ │ ├─┤     ║
    // ║     ╚═╝└─┘┴  ┴ ┴┴└─ ╩ └─┘┴ └─ ┴ └─┘┴└─└─┘╚═╝┴└─┴ ┴┴  ┴ ┴┴└─┘└─┘═╩╝┴ ┴ ┴ ┴ ┴     ║
    // ║                                                                                 ║
    // ╚═════════════════════════════════════════════════════════════════════════════════╝
    SeparTextureGraphicsData::~SeparTextureGraphicsData()
    {
        privateClear();
    }

    SeparTextureGraphicsData::SeparTextureGraphicsData(SeparTextureGraphicsData&& other) noexcept
        : BaseTextureGraphicsData(std::move(other)),
          _textureVbo(other._textureVbo)
    {
        other._textureVbo = 0;
    }

    SeparTextureGraphicsData& SeparTextureGraphicsData::operator=(
        SeparTextureGraphicsData&& other) noexcept
    {
        if (this == &other) [[unlikely]]
        {
            return *this;
        }

        SeparTextureGraphicsData tmp(std::move(other));
        swap(*this, tmp);
        return *this;
    }

    void SeparTextureGraphicsData::generate()
    {
        if (Verify(!isValid())) [[likely]]
        {
            BaseTextureGraphicsData::generate();
            glGenBuffers(1, &_textureVbo);
        }
    }

    void SeparTextureGraphicsData::clear()
    {
        BaseTextureGraphicsData::clear();
        privateClear();
    }

    bool SeparTextureGraphicsData::isValid() const noexcept
    {
        return BaseTextureGraphicsData::isValid() && _textureVbo != 0;
    }

    void SeparTextureGraphicsData::setTextureVertexBuffer(const std::vector<float>& data,
                                                          GLenum usage)
    {
        if (Verify(_textureVbo != 0 && getVaoId() != 0)) [[likely]]
        {
            bindVAO();
            bindTextureVBO();
            glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(sizeof(float) * data.size()),
                         data.data(), GL_STATIC_DRAW);
        }
    }

    StringAtom SeparTextureGraphicsData::getCacheHash() const
    {
        return "SeparTextureGraphicsData"_atom;
    }

    void SeparTextureGraphicsData::privateClear()
    {
        glDeleteBuffers(1, &_textureVbo);
        _textureVbo = 0;
    }

    // ╔════════════════════════════╗
    // ║                            ║
    // ║     ╔═╗┬  ┌─┐┌┐ ┌─┐┬       ║
    // ║     ║ ╦│  │ │├┴┐├─┤│       ║
    // ║     ╚═╝┴─┘└─┘└─┘┴ ┴┴─┘     ║
    // ║                            ║
    // ╚════════════════════════════╝
    void to_json(nlohmann::json& j, const Core::BaseGraphicsData::ModifierParam& v)
    {
        j["modifier"] = R<BaseGraphicsData::Modifier>::ToString(v.modifier);
        j["value"] = R<BaseGraphicsData::ModifiedValue>::ToString(v.value);
    }

    void from_json(const nlohmann::json& j, Core::BaseGraphicsData::ModifierParam& v)
    {
        v.modifier = R<BaseGraphicsData::Modifier>::FromString(j["modifier"].get<std::string>())
                         .value_or(BaseGraphicsData::Modifier::None);
        v.value = R<BaseGraphicsData::ModifiedValue>::FromString(j["value"].get<std::string>())
                      .value_or(BaseGraphicsData::ModifiedValue::None);
    }

} // namespace Core
