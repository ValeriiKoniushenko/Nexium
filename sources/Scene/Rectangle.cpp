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
#include "Core/Assert.h"
#include "GameplaySystem/Camera.h"
#include "GameplaySystem/Framework/GameInstance.h"
#include "Graphics/GraphicsComponents.h"
#include "Graphics/Line.h"

using namespace Core;

namespace
{
    [[nodiscard]] BaseGraphicsData GetDefaultGraphicsData(float defSize)
    {
        std::vector<BaseGraphicsData::ModifierParam> modifiers
            = { { .value = BaseGraphicsData::ModifiedValue::CullFace,
                  .modifier = BaseGraphicsData::Modifier::Disable } };

        const float w = defSize;
        const float h = defSize;

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

        auto* shader = GetShaderManager()->getShaderProgram("2d_rect"_atom);
        if (!Verify(shader))
        {
            globalLog.criticalLog("Can't get shader program '2d_rect'.");
            return {};
        }
        data.setShader(shader);

        data.setDrawModifiers(std::move(modifiers));

        data.setVertexBuffer(vert);
        data.setIndexBuffer(ind);
        shader->callSetEvent(ShaderProgram::Event::OnSetIndexAndVertexBuffer);

        return data;
    }

} // namespace

namespace Core::SceneObj
{

    ECS_IMPL(Rectangle);
    ECS_IMPL(RectangleAnimated);

    FSize2 Rectangle::getDrawRectSize() const noexcept
    {
        return FSize2(GetDefaultDrawRectSize() * glm::vec2(_scale));
    }

    void Rectangle::tryDrawOutline(BaseCamera& camera)
    {
        // if (!shouldDrawOutline())
        // {
        //     return;
        // }

        const auto* shader = GetShaderManager()->getShaderProgram("line"_atom);

        if (!shader) [[unlikely]]
        {
            LOG_CRITICAL_ONCE("Can't get shader program 'line'.");
            AssertOnce(false);
            return;
        }

        Debug::Line::Draw(shader, camera.getMatrix(), glm::vec3(0.f), glm::vec3(100.f));
    }

    void Rectangle::onDraw(BaseCamera& camera)
    {
        static BaseGraphicsData gcd = GetDefaultGraphicsData(Rectangle::GetDefaultDrawRectSize());
        tryToRecalculateMatrices();

        auto& atlas = GetAssetsManager()->getAtlas(_atlasName);
        auto* shader = GetShaderManager()->getShaderProgram("2d_rect"_atom);
        if (!shader) [[unlikely]]
        {
            AssertOnce(false);
            LOG_CRITICAL_ONCE("Can't get shader program '2d_rect'.");
            return;
        }

        shader->use();
        shader->setUniform("uTexture"_atom, 0);
        shader->setUniform("uProjAndView"_atom, camera.getMatrix());

        atlas.bind();

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glm::vec2 textureOffset{ 0.f, 0.f };
        glm::vec2 textureSize{ 1.f, 1.f };
        if (!_textureName.isEmpty())
        {
            const auto rect = atlas.getRect(_textureName);
            textureOffset = rect.getLeftTop();
            textureSize = rect.getRightBottom() - textureOffset;
        }

        auto modelMatrix = getModelMatrix();

        // Converting Left-Bottom origin -> Left-Top origin
        modelMatrix[3][1] -= GetDefaultDrawRectSize();

        shader->setUniform("uUVOffset"_atom, textureOffset + textureSize * _textureUVOffset);
        shader->setUniform("uUVSize"_atom, textureSize * _textureUVSize);
        shader->setUniform("uModel"_atom, modelMatrix);
        shader->setUniform("uAlphaBlendingEnabled"_atom, static_cast<GLint>(_blendingEnabled));

        glBlendFunc(_blendingEnabled ? GL_SRC_ALPHA : GL_ONE,
                    _blendingEnabled ? GL_ONE_MINUS_SRC_ALPHA : GL_ZERO);
        gcd.directDraw();

        tryDrawOutline(camera);
    }

    nlohmann::json Rectangle::getTypeSpecificSceneDataAsJson() const
    {
        auto out = SceneObject::getTypeSpecificSceneDataAsJson();

        out["_textureName"] = _textureName;
        out["_atlasName"] = _atlasName;
        out["_blendingEnabled"] = _blendingEnabled;
        return out;
    }

    void Rectangle::applyTypeSpecificSceneData(const nlohmann::json& data)
    {
        SceneObject::applyTypeSpecificSceneData(data);

        if (data.contains("_textureName"))
        {
            _textureName = StringAtom::Intern(data.at("_textureName").get<StringAtom>());
        }

        if (data.contains("_atlasName"))
        {
            _atlasName = StringAtom::Intern(data.at("_atlasName").get<StringAtom>());
        }

        _blendingEnabled = data.value("_blendingEnabled", true);
    }

    void Rectangle::setTexture(const StringAtom& value)
    {
        _textureName = value;
        resetTextureUV();
    }

    void Rectangle::setTextureUV(glm::vec2 offset, glm::vec2 size)
    {
        _textureUVOffset = offset;
        _textureUVSize = size;
    }

    void Rectangle::resetTextureUV() noexcept
    {
        _textureUVOffset = { 0.f, 0.f };
        _textureUVSize = { 1.f, 1.f };
    }

    void RectangleAnimated::setAnimationEnabled(bool value)
    {
        _animationEnabled = value;

        auto* animator = findFirstChildOf<Animation::FrameByFrameAnimator>();
        if (!animator)
        {
            return;
        }

        animator->setEnabled(value);
        if (!value)
        {
            if (auto* animation = animator->getActiveAnimation())
            {
                animation->stop();
            }
            resetTextureUV();
            return;
        }

        if (!_animationOverrideName.isEmpty())
        {
            animator->startAnimation(_animationOverrideName);
        }
        else if (!animator->getActiveAnimationName().isEmpty())
        {
            animator->startAnimation(animator->getActiveAnimationName());
        }
    }

    void RectangleAnimated::setAnimationOverride(const StringAtom& animationName, float fps)
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
            if (_animationEnabled)
            {
                animator->startAnimation(animationName);
            }
        }
    }

    const StringAtom& RectangleAnimated::getAnimationOverrideName() const noexcept
    {
        return _animationOverrideName;
    }

    float RectangleAnimated::getAnimationOverrideFPS() const noexcept
    {
        return _animationOverrideFPS;
    }

    nlohmann::json RectangleAnimated::getTypeSpecificSceneDataAsJson() const
    {
        auto out = Rectangle::getTypeSpecificSceneDataAsJson();
        out["_animationEnabled"] = _animationEnabled;
        if (!_animationOverrideName.isEmpty() && _animationOverrideFPS > 0.f)
        {
            out["_animationName"] = _animationOverrideName;
            out["_animationFPS"] = _animationOverrideFPS;
        }
        return out;
    }

    void RectangleAnimated::applyTypeSpecificSceneData(const nlohmann::json& data)
    {
        Rectangle::applyTypeSpecificSceneData(data);

        _animationEnabled = data.value("_animationEnabled", true);

        if (data.contains("_animationName") && data.contains("_animationFPS"))
        {
            setAnimationOverride(data.at("_animationName").get<StringAtom>(),
                                 data.at("_animationFPS").get<float>());
        }

        setAnimationEnabled(_animationEnabled);
    }

} // namespace Core::SceneObj
