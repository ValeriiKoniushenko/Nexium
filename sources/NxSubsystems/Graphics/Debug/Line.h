// Nexium
// Copyright 2018-2027 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "Core/Color.h"
#include "glm/fwd.hpp"
#include "glm/vec3.hpp"

namespace NX
{
    class ShaderProgram;
}

namespace NX::Debug
{

    class Line
    {
    public:
        Line() = delete;

        static void Draw(const ShaderProgram* shader, const glm::mat4& projViewMatric,
                         const glm::vec3& start, const glm::vec3& end, float thickness = 4.f,
                         const Core::NormColor4& color = Core::Color4_Yellow);

        static void Draw(const ShaderProgram* shader, const glm::mat4& projViewMatric,
                         glm::vec2 start, glm::vec2 end, float thickness = 4.f,
                         const Core::NormColor4& color = Core::Color4_Yellow);
    };

} // namespace NX::Debug