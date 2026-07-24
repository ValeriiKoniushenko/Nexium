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

#include "Rectangle.h"

#include "GameplaySystem/Camera.h"
#include "GameplaySystem/Framework/GameInstance.h"
#include "Graphics/GraphicsComponents.h"

namespace Core::SceneObj
{

    ECS_IMPL(Rectangle);

    void Rectangle::draw(BaseCamera& camera)
    {
        static BaseGraphicsData gcd = []()
        {
            std::vector<BaseGraphicsData::ModifierParam> modifiers
                = { { .value = BaseGraphicsData::ModifiedValue::CullFace,
                      .modifier = BaseGraphicsData::Modifier::Disable } };

            float w = 100.f;
            float h = 50.f;

            const std::vector<float> vert = {
                0, h, 0, // 0  top-left         | (0 , 64) * (0, 1)
                0, 0, 0, // 1  bottom-left      | (0 , 0 ) * (0, 0)
                w, 0, 0, // 2  bottom-right     | (64, 0 ) * (1, 0)
                w, h, 0  // 3  top-right        | (64, 64) * (1, 1)
            };

            const std::vector<GLuint> ind = {
                0, 1, 2, // triangle 1
                2, 3, 0  // triangle 2
            };

            BaseGraphicsData data;
            data.generate();

            auto* shader = GetShaderManager().getShaderProgram("2d_rect"_atom);
            data.setShader(shader);

            data.setDrawModifiers(std::move(modifiers));

            data.setVertexBuffer(vert);
            data.setIndexBuffer(ind);
            shader->callSetEvent(ShaderProgram::Event::OnSetIndexAndVertexBuffer);

            return data;
        }();
        tryToRecalculateMatrices();

        auto& atlas = GetAssetsManager().getAtlas(_atlasName);
        auto* shader = GetShaderManager().getShaderProgram("2d_rect"_atom);
        shader->use();
        shader->setUniform("uTexture"_atom, 0);
        shader->setUniform("uProjAndView"_atom, camera.getMatrix());

        atlas.bind();

        const auto rect = atlas.getRect(_textureName);
        const auto textureOffset = rect.getLeftTop();
        const auto textureSize = rect.getRightBottom() - textureOffset;

        shader->setUniform("uUVOffset"_atom, textureOffset + textureSize * _textureUVOffset);
        shader->setUniform("uUVSize"_atom, textureSize * _textureUVSize);
        shader->setUniform("uModel"_atom, getModelMatrix());

        gcd.directDraw();
    }

    nlohmann::json Rectangle::getTypeSpecificSceneDataAsJson() const
    {
        auto out = SceneObject::getTypeSpecificSceneDataAsJson();
        out["_textureName"] = _textureName;
        out["_atlasName"] = _atlasName;
        return out;
    }

    void Rectangle::applyTypeSpecificSceneData(const nlohmann::json& data)
    {
        if (data.contains("_textureName"))
        {
            _textureName = StringAtom::Intern(data.at("_textureName").get<StringAtom>());
        }

        if (data.contains("_atlasName"))
        {
            _atlasName = StringAtom::Intern(data.at("_atlasName").get<StringAtom>());
        }
    }

} // namespace Core::SceneObj
