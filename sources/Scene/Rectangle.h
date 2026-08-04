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
        [[nodiscard]] StringAtom getTextureName() const { return _textureName; }
        void setTexture(const StringAtom& value) { _textureName = value; }
        void setTextureUV(GlobalPosition2F offset, GlobalPosition2F size)
        {
            _textureUVOffset = std::move(offset);
            _textureUVSize = std::move(size);
        }

        [[nodiscard]] StringAtom getAtlasName() const { return _atlasName; }
        void setAtlas(const StringAtom& value) { _atlasName = value; }

        [[nodiscard]] bool isBlendingEnabled() const noexcept { return _blendingEnabled; }
        void setBlendingEnabled(bool value) noexcept { _blendingEnabled = value; }

        void setAnimationOverride(const StringAtom& animationName, float fps);
        [[nodiscard]] const StringAtom& getAnimationOverrideName() const noexcept
        {
            return _animationOverrideName;
        }
        [[nodiscard]] float getAnimationOverrideFPS() const noexcept
        {
            return _animationOverrideFPS;
        }

        [[nodiscard]] nlohmann::json getTypeSpecificSceneDataAsJson() const override;
        void applyTypeSpecificSceneData(const nlohmann::json& data) override;

    protected:
        void onDraw(BaseCamera& camera) override;

    protected:
        FIELD();
        StringAtom _textureName = "red.png"_atom;

        FIELD();
        StringAtom _atlasName = "default"_atom;

        GlobalPosition2F _textureUVOffset{ 0.f, 0.f };
        GlobalPosition2F _textureUVSize{ 1.f, 1.f };

        bool _blendingEnabled = true;

        StringAtom _animationOverrideName;
        float _animationOverrideFPS = 0.f;
    };

} // namespace Core::SceneObj

#include "Rectangle.generated.h" // added by the code generator. Better don't move it.
