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

#include "SkyboxAsset.h"

#include "GameplaySystem/Framework/GameInstance.h"
#include "Misc/Configs.h"

namespace Core
{
    R_FRIEND_IMPL(SkyboxAsset);

    void SkyboxAsset::draw(BaseCamera& camera)
    {
        auto* shader = GetShaderManager()->getShaderProgram("skybox"_atom);

        auto view = glm::mat4(1.f);
        view = glm::rotate(view, glm::radians(camera.getGlobalRotation().x),
                           glm::vec3(1.f, 0.f, 0.f));
        view = glm::rotate(view, glm::radians(camera.getGlobalRotation().y),
                           glm::vec3(0.f, 1.f, 0.f));

        shader->use();
        shader->setUniform("uView"_atom, view);
        shader->setUniform("uProj"_atom, camera.getCachedProjectionMatrix());

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

        auto* shader = GetShaderManager()->getShaderProgram("skybox"_atom);

        _gcd.generate();
        _gcd.setShader(GetShaderManager()->getShaderProgram("skybox"_atom));
        _gcd.setVertexBuffer(skyboxVertices);
        _gcd.setIndexBuffer(skyboxIndices);
        _gcd.setDrawModifiers(std::move(modifiers));

        shader->use();
        shader->setUniform("uSkybox"_atom, 0);

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

                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, img.getChannelAsOpenGLType(),
                             img.getSize().width, img.getSize().height, 0,
                             img.getChannelAsOpenGLType(), GL_UNSIGNED_BYTE, img.data());
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
    }

} // namespace Core