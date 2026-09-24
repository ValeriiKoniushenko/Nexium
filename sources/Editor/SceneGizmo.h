// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

// Module: Nexium::Editor

#pragma once

#include "glm/glm.hpp"

namespace NX
{
    class BaseCamera;
    class SceneObject;
    class Transformable;

    class SceneGizmo
    {
    public:
        void draw(SceneObject& object, BaseCamera& camera, glm::vec2 position, glm::vec2 size);
        [[nodiscard]] bool blocksPicking() const noexcept { return _blocksPicking; }

        static bool ApplyTransform(Transformable& object, const glm::mat4& worldPivot,
                                   const glm::mat4& parent, bool rotate);

    private:
        bool _blocksPicking = false;
    };
} // namespace NX
