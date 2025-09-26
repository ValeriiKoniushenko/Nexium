/*
 * MIT License
 *
 * Copyright (c) 2018-2025 Valerii Koniushenko
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

namespace Core
{
    void SkyboxAsset::draw()
    {
        auto* shader = GetShaderManager().getShaderProgram("skybox"_atom);
        auto* camera = gGameInstance->currentCamera;

        auto view = glm::mat4(1.f);
        view = glm::rotate(view, glm::radians(camera->getRotationX()), glm::vec3(1.f, 0.f, 0.f));
        view = glm::rotate(view, glm::radians(camera->getRotationY()), glm::vec3(0.f, 1.f, 0.f));
        shader->use();
        shader->setUniform("uView"_atom, view);
        shader->setUniform("uProj"_atom, camera->getCachedProjectionMatrix());

        glDisable(GL_CULL_FACE);
        glDepthFunc(GL_LEQUAL);

        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_CUBE_MAP, _cubeMapId);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glDepthFunc(GL_LESS);
        glEnable(GL_CULL_FACE);
    }

    void SkyboxAsset::onLoadRequest()
    {
        // clang-format off
        float skyboxVertices[] = {
            // positions
            -1.0f,  1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

             1.0f, -1.0f, -1.0f,
             1.0f, -1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

            -1.0f,  1.0f, -1.0f,
             1.0f,  1.0f, -1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
             1.0f, -1.0f,  1.0f
        };
        // clang-format on

        auto* shader = GetShaderManager().getShaderProgram("skybox"_atom);
        shader->use();

        glGenVertexArrays(1, &skyboxVAO);
        glGenBuffers(1, &skyboxVBO);
        glGenTextures(1, &_cubeMapId);

        glBindVertexArray(skyboxVAO);
        glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);

        glBindTexture(GL_TEXTURE_CUBE_MAP, _cubeMapId);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);

        int size = -1;
        for (std::size_t i = 0; i < _paths.size(); ++i)
        {
            Image img;
            if (img.loadFromFile(Config::Path::assets / _paths[i], _isFlipVertically))
            {
                if (size == -1)
                {
                    size = img.getSize().width;
                }

                if (size != img.getSize().height || size != img.getSize().width)
                {
                    criticalLog(
                        "Incorrect the cube-map's texture size. Size of different sections are different.");
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
        shader->setUniform("uSkybox"_atom, 1);
        glBindVertexArray(0);

        traceLog("Loaded: " + _logicPath);
    }

    void SkyboxAsset::onUnloadRequest()
    {
        traceLog("Unloaded: " + _logicPath);

        glDeleteVertexArrays(1, &skyboxVAO);
        glDeleteBuffers(1, &skyboxVBO);
        glDeleteTextures(1, &_cubeMapId);
        skyboxVAO = skyboxVBO = _cubeMapId = 0;
    }

    void SkyboxAsset::onFillData(nlohmann::json&& json)
    {
        auto getPath = [this, &json](const char* prop, Direction dir)
        {
            if (Verify(json.contains(prop)))
            {
                _paths.at(static_cast<std::size_t>(dir)) = json[prop].get<std::filesystem::path>();
            }
        };

        getPath("pathToTop", Direction::Top);
        getPath("pathToBottom", Direction::Bottom);
        getPath("pathToBack", Direction::Back);
        getPath("pathToFront", Direction::Front);
        getPath("pathToLeft", Direction::Left);
        getPath("pathToRight", Direction::Right);

        if (json.contains("isFlipVertically"))
        {
            _isFlipVertically = json["isFlipVertically"].get<bool>();
        }
    }
} // namespace Core