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

#include "Animations/FrameByFrame/FrameByFrameAnimator.h"
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

        glm::vec2 textureOffset{0.f, 0.f};
        glm::vec2 textureSize{1.f, 1.f};
        if (!_textureName.isEmpty())
        {
            const auto rect = atlas.getRect(_textureName);
            textureOffset = rect.getLeftTop();
            textureSize = rect.getRightBottom() - textureOffset;
        }

        shader->setUniform("uUVOffset"_atom, textureOffset + textureSize * _textureUVOffset);
        shader->setUniform("uUVSize"_atom, textureSize * _textureUVSize);
        shader->setUniform("uModel"_atom, getModelMatrix());
        shader->setUniform("uAlphaBlendingEnabled"_atom,
                           static_cast<GLint>(_blendingEnabled));

        glBlendFunc(_blendingEnabled ? GL_SRC_ALPHA : GL_ONE,
                    _blendingEnabled ? GL_ONE_MINUS_SRC_ALPHA : GL_ZERO);
        gcd.directDraw();
    }

    nlohmann::json Rectangle::getTypeSpecificSceneDataAsJson() const
    {
        auto out = SceneObject::getTypeSpecificSceneDataAsJson();
        out["_textureName"] = _textureName;
        out["_atlasName"] = _atlasName;
        out["_blendingEnabled"] = _blendingEnabled;
        if (!_animationOverrideName.isEmpty() && _animationOverrideFPS > 0.f)
        {
            out["_animationName"] = _animationOverrideName;
            out["_animationFPS"] = _animationOverrideFPS;
        }
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

        _blendingEnabled = data.value("_blendingEnabled", true);

        if (data.contains("_animationName") && data.contains("_animationFPS"))
        {
            setAnimationOverride(data.at("_animationName").get<StringAtom>(),
                                 data.at("_animationFPS").get<float>());
        }
    }

    void Rectangle::setAnimationOverride(const StringAtom& animationName, float fps)
    {
        _animationOverrideName = animationName;
        _animationOverrideFPS = fps;

        auto* animator = findFirstChildOf<Animation::FrameByFrameAnimator>();
        if (!animator || animationName.isEmpty() || fps <= 0.f)
        {
            return;
        }
        if (auto* animation = animator->getAnimation(animationName))
        {
            animation->setFPS(fps);
            animator->startAnimation(animationName);
        }
    }

} // namespace Core::SceneObj
