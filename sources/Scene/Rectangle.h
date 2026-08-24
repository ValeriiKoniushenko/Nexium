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

#pragma once

#include "Scene/SceneObject.h"

#include <utility>

namespace Core::SceneObj
{

    CLASS();
    class Rectangle : public SceneObject
    {
        ECS_DECL(Rectangle, Core::SceneObject);

    public:
        ~Rectangle() override = default;
        Rectangle(const Rectangle&) = default;
        Rectangle(Rectangle&&) noexcept = default;
        Rectangle& operator=(const Rectangle&) = default;
        Rectangle& operator=(Rectangle&&) noexcept = default;

        [[nodiscard]] StringAtom getTextureName() const { return _textureName; }
        void setTexture(const StringAtom& value);
        void setTextureUV(glm::vec2 offset, glm::vec2 size);
        void resetTextureUV() noexcept;

        [[nodiscard]] bool isBlendingEnabled() const noexcept { return _blendingEnabled; }
        void setBlendingEnabled(bool value) noexcept { _blendingEnabled = value; }

        [[nodiscard]] nlohmann::json getTypeSpecificSceneDataAsJson() const override;
        void applyTypeSpecificSceneData(const nlohmann::json& data) override;

        [[nodiscard]] constexpr static float GetDefaultDrawRectSize() noexcept { return 100.f; }
        [[nodiscard]] FSize2 getDrawRectSize() const noexcept;

    protected:
        void onDraw(BaseCamera& camera) override;

    protected:
        FIELD();
        StringAtom _textureName = "red.png"_atom;

        FIELD();
        StringAtom _atlasName = "default"_atom;

        glm::vec2 _textureUVOffset{ 0.f, 0.f };
        glm::vec2 _textureUVSize{ 1.f, 1.f };

        bool _blendingEnabled = true;
    };

    CLASS();
    class RectangleAnimated : public Rectangle
    {
        ECS_DECL(RectangleAnimated, Core::SceneObj::Rectangle);

    public:
        ~RectangleAnimated() override = default;
        RectangleAnimated(const RectangleAnimated&) = default;
        RectangleAnimated(RectangleAnimated&&) noexcept = default;
        RectangleAnimated& operator=(const RectangleAnimated&) = default;
        RectangleAnimated& operator=(RectangleAnimated&&) noexcept = default;

        [[nodiscard]] StringAtom getAtlasName() const { return _atlasName; }
        void setAtlas(const StringAtom& value) { _atlasName = value; }

        [[nodiscard]] bool isAnimationEnabled() const noexcept { return _animationEnabled; }
        void setAnimationEnabled(bool value);

        void setAnimationOverride(const StringAtom& animationName, float fps);
        [[nodiscard]] const StringAtom& getAnimationOverrideName() const noexcept;
        [[nodiscard]] float getAnimationOverrideFPS() const noexcept;

        [[nodiscard]] nlohmann::json getTypeSpecificSceneDataAsJson() const override;
        void applyTypeSpecificSceneData(const nlohmann::json& data) override;

    protected:
        bool _animationEnabled = true;

        StringAtom _animationOverrideName;
        float _animationOverrideFPS = 0.f;
    };

} // namespace Core::SceneObj

#include "Rectangle.generated.h" // added by the code generator. Better don't move it.
