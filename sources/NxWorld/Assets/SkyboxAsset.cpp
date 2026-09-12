// Nexium
// Copyright 2018-2027 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "SkyboxAsset.h"

#include "Foundation/Configs.h"
#include "RawBackend/Image.h"

using namespace RawBackend;
using namespace Foundation;

namespace
{
    GLenum GetChannelAsOpenGLType(const RawBackend::Image& data) noexcept
    {
        if (static_cast<int>(data.getChannel()) == 3)
        {
            return GL_RGB;
        }

        if (static_cast<int>(data.getChannel()) == 4)
        {
            return GL_RGBA;
        }

        return GL_RED;
    }
} // namespace

namespace NX
{
    R_FRIEND_IMPL(SkyboxAsset);

    void SkyboxAsset::draw(BaseCamera& camera, ShaderProgram& shader)
    {
        if (_shader != &shader)
        {
            _shader = &shader;
            _gcd.setShader(_shader);
        }

        auto view = glm::mat4(1.f);
        view = glm::rotate(view, glm::radians(camera.getGlobalRotation().x),
                           glm::vec3(1.f, 0.f, 0.f));
        view = glm::rotate(view, glm::radians(camera.getGlobalRotation().y),
                           glm::vec3(0.f, 1.f, 0.f));

        shader.use();
        shader.setUniform("uSkybox"_atom, 0);
        shader.setUniform("uView"_atom, view);
        shader.setUniform("uProj"_atom, camera.getCachedProjectionMatrix());

        glDepthFunc(GL_LEQUAL);
        _gcd.directDraw(GL_TRIANGLES, GL_TEXTURE_CUBE_MAP, 0);
        glDepthFunc(GL_LESS);
    }

    void SkyboxAsset::onLoadRequest()
    {
        // clang-format off
        const std::vector<float> skyboxVertices = {
            -1.0f,  1.0f, -1.0f,  // 0
            -1.0f, -1.0f, -1.0f,  // 1
             1.0f, -1.0f, -1.0f,  // 2
             1.0f,  1.0f, -1.0f,  // 3
            -1.0f,  1.0f,  1.0f,  // 4
            -1.0f, -1.0f,  1.0f,  // 5
             1.0f, -1.0f,  1.0f,  // 6
             1.0f,  1.0f,  1.0f   // 7
        };
        const std::vector<GLuint> skyboxIndices = {
            // back face
            0, 1, 2,
            2, 3, 0,
            // left face
            1, 5, 4,
            4, 0, 1,
            // right face
            2, 6, 7,
            7, 3, 2,
            // front face
            5, 6, 7,
            7, 4, 5,
            // top face
            4, 7, 3,
            3, 0, 4,
            // bottom face
            1, 2, 6,
            6, 5, 1
        };
        // clang-format on

        std::vector<BaseGraphicsData::ModifierParam> modifiers
            = { { .value = BaseGraphicsData::ModifiedValue::CullFace,
                  .modifier = BaseGraphicsData::Modifier::Disable } };

        _gcd.generate();
        _gcd.setVertexBuffer(skyboxVertices);
        _gcd.setIndexBuffer(skyboxIndices);
        _gcd.setDrawModifiers(std::move(modifiers));

        _gcd.bindVAO();
        _gcd.bindTexture(GL_TEXTURE_CUBE_MAP);

        for (const auto& path : _paths)
        {
            if (path.empty()) [[unlikely]]
            {
                _gcd.unbindVao();
                errorLog("One of Skybox's path is empty!");
                return;
            }
        }

        int size = -1;
        for (std::size_t i = 0; i < _paths.size(); ++i)
        {
            Image img;
            if (img.loadFromFile(Config::Path::projectAbsPath / _paths[i], _isFlipVertically))
            {
                if (size == -1)
                {
                    size = img.getSize().width;
                }

                if (size != img.getSize().height || size != img.getSize().width)
                {
                    criticalLog(
                        "Incorrect the cube-map's texture size. Size of different sections are "
                        "different.");
                }

                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
                             static_cast<GLint>(GetChannelAsOpenGLType(img)), img.getSize().width,
                             img.getSize().height, 0, GetChannelAsOpenGLType(img), GL_UNSIGNED_BYTE,
                             img.data());
            }
        }

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        _gcd.unbindVao();
    }

    void SkyboxAsset::onUnloadRequest()
    {
        _gcd.clear();
        _shader = nullptr;
    }

} // namespace NX
