// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "../SceneObject.h"

#include <utility>

namespace NX
{
    class BaseGraphicsData;
}
namespace NX::SceneObj
{

    CLASS();
    class Rectangle : public SceneObject
    {
        ECS_DECL(Rectangle, NX::SceneObject);

    public:
        ~Rectangle() override = default;
        Rectangle(const Rectangle&) = default;
        Rectangle(Rectangle&&) noexcept = default;
        Rectangle& operator=(const Rectangle&) = default;
        Rectangle& operator=(Rectangle&&) noexcept = default;

        [[nodiscard]] Core::StringAtom getTextureName() const { return _textureName; }
        void setTexture(const Core::StringAtom& value);
        void setTextureUV(glm::vec2 offset, glm::vec2 size);
        void resetTextureUV() noexcept;

        [[nodiscard]] bool isBlendingEnabled() const noexcept { return _blendingEnabled; }
        void setBlendingEnabled(bool value) noexcept { _blendingEnabled = value; }

        [[nodiscard]] nlohmann::json getTypeSpecificSceneDataAsJson() const override;
        void applyTypeSpecificSceneData(const nlohmann::json& data) override;

        [[nodiscard]] constexpr static float GetDefaultDrawRectSize() noexcept { return 100.f; }
        [[nodiscard]] Core::FSize2 getDrawRectSize() const noexcept;

    protected:
        void tryDrawOutline(BaseCamera& camera);
        void onDraw(BaseCamera& camera) override;

    protected:
        FIELD();
        Core::StringAtom _textureName = "red.png"_atom;

        FIELD();
        Core::StringAtom _atlasName = "default"_atom;

        glm::vec2 _textureUVOffset{ 0.f, 0.f };
        glm::vec2 _textureUVSize{ 1.f, 1.f };

        bool _blendingEnabled = true;
    };

} // namespace NX::SceneObj

#include "Rectangle.generated.h"
