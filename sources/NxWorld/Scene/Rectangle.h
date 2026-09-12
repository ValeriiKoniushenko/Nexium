// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "Scene/SceneObject.h"

#include <utility>

namespace Core
{
    class BaseGraphicsData;
}
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
        void tryDrawOutline(BaseCamera& camera);
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
