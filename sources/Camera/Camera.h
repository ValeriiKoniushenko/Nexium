// MIT License
//
// Copyright (c) 2019-2025 Valerii Koniushenko
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include "ModuleInfo.h"
#include "glm/glm.hpp"

namespace SW
{
    class BaseCamera : public PS::BaseLog
    {
    public:
        [[nodiscard]] spdlog::logger* getLogger() const override { return Camera::getLogger(); }

        BaseCamera();

        [[nodiscard]] glm::vec3 getPosition() const;

        void setPosition(const glm::vec3& position);

        void rotate(const glm::vec3& value);

        [[nodiscard]] const glm::mat4& getMatrix();

        void setFov(float fov);
        [[nodiscard]] float getFov() const;

        void setNear(float value);
        [[nodiscard]] float getNear() const;

        void setFar(float value);
        [[nodiscard]] float getFar() const;

        void setSensitive(glm::vec2 value);
        [[nodiscard]] glm::vec2 getSensitive() const;

    protected:
        glm::mat4 _cachedProjMatrix = glm::mat4(1.f);
        glm::mat4 _cachedCalculatedMatrix = glm::mat4(1.f);
        glm::mat4 _cachedViewMatrix = glm::mat4(1.f);
        glm::vec2 _sensitive{ 3.f, 3.f };
        glm::vec3 _position{};
        float fov_ = 90.f;
        float far_ = 10'000.f;
        float near_ = 1.f;
    };
} // namespace SW