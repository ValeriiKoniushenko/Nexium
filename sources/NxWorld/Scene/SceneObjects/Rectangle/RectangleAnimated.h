// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "Rectangle.h"

namespace NX::SceneObj
{
    CLASS();
    class RectangleAnimated : public Rectangle
    {
        ECS_DECL(RectangleAnimated, NX::SceneObj::Rectangle);

    public:
        ~RectangleAnimated() override = default;
        RectangleAnimated(const RectangleAnimated&) = default;
        RectangleAnimated(RectangleAnimated&&) noexcept = default;
        RectangleAnimated& operator=(const RectangleAnimated&) = default;
        RectangleAnimated& operator=(RectangleAnimated&&) noexcept = default;

        [[nodiscard]] Core::StringAtom getAtlasName() const { return _atlasName; }
        void setAtlas(const Core::StringAtom& value) { _atlasName = value; }

        [[nodiscard]] bool isAnimationEnabled() const noexcept { return _animationEnabled; }
        void setAnimationEnabled(bool value);

        void setAnimationOverride(const Core::StringAtom& animationName, float fps);
        [[nodiscard]] const Core::StringAtom& getAnimationOverrideName() const noexcept;
        [[nodiscard]] float getAnimationOverrideFPS() const noexcept;

        [[nodiscard]] nlohmann::json getTypeSpecificSceneDataAsJson() const override;
        void applyTypeSpecificSceneData(const nlohmann::json& data) override;

    protected:
        bool _animationEnabled = true;

        Core::StringAtom _animationOverrideName;
        float _animationOverrideFPS = 0.f;
    };

} // namespace NX::SceneObj

#include "RectangleAnimated.generated.h"
