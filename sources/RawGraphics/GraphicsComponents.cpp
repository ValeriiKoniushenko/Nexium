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
        _shader = 0;
        _triangleCount = 0;
    }

    void GraphicsComponentData::directDraw() const noexcept
    {
        if (!isValid()) [[unlikely]]
        {
            Assert("Can't draw graphic component. It wasn't configured.");
            return;
        }

        for (auto modifier : _drawModifiers)
        {
        }

        _shader->use();
        glBindVertexArray(_vao);
        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, _texture);

        glDrawElements(GL_TRIANGLES, _triangleCount, GL_UNSIGNED_INT, 0);

        for (auto modifier : _drawModifiers)
        {
        }
    }
} // namespace SW