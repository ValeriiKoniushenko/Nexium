// Nexium
// Copyright 2018-2027 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "Core/Size.h"

namespace Core
{
    class Scene;

    class ApplicationIntegration
    {
    public:
        virtual ~ApplicationIntegration() = default;

        virtual void initialize() = 0;
        virtual void readFromCache() = 0;
        virtual void writeToCache() = 0;
        virtual void updateInput() = 0;
        virtual void tick(float delta) = 0;
        virtual void updateSceneInteraction(Scene& scene) = 0;
        [[nodiscard]] virtual bool isViewportFocused() const = 0;
        virtual void beforeSceneDraw() = 0;
        virtual void afterSceneDraw() = 0;
        virtual void clearSceneRenderTarget() = 0;
        [[nodiscard]] virtual ISize2 getRenderSize() const = 0;
    };
} // namespace Core
