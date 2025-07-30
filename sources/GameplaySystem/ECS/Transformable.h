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

#include "Misc/JsonAdapter.h"
#include "glm/glm.hpp"

#include <Core/Position.h>

namespace SW
{
    using GPos3 = Core::GlobalPosition3F;
    using GPos2 = Core::GlobalPosition2F;
    using RPos3 = Core::RelativePosition3F;
    using RPos2 = Core::RelativePosition2F;

    class Transformable : public JsonAdapter
    {
    public:
        Transformable() = default;
        ~Transformable() override = default;

        Transformable(const Transformable&) = default;
        Transformable(Transformable&&) noexcept = default;
        Transformable& operator=(const Transformable&) = default;
        Transformable& operator=(Transformable&&) noexcept = default;

        [[nodiscard]] GPos3 getPosition() const noexcept;
        void setPosition(const GPos3& position) noexcept;
        void moveForward(float offset) noexcept;
        void moveRight(float offset) noexcept;
        void moveUp(float offset) noexcept;

        void setRotation(const glm::vec3& rotation) noexcept;
        void rotate(const glm::vec3& value) noexcept;
        [[nodiscard]] const glm::vec3& getRotation() const noexcept;
        [[nodiscard]] glm::vec3& getRotation() noexcept;
        void setRotationX(float x) noexcept;
        void rotateX(float x) noexcept;
        [[nodiscard]] float getRotationX() const noexcept;
        void setRotationY(float y) noexcept;
        void rotateY(float y) noexcept;
        [[nodiscard]] float getRotationY() const noexcept;
        void setRotationZ(float z) noexcept;
        void rotateZ(float z) noexcept;
        [[nodiscard]] float getRotationZ() const noexcept;

        void setOrigin(const glm::vec3& origin) noexcept;
        [[nodiscard]] const glm::vec3& getOrigin() const noexcept;

        void scale(const glm::vec3& value) noexcept;
        void setScale(const glm::vec3& value) noexcept;
        [[nodiscard]] glm::vec3 getScale() const noexcept;

        [[nodiscard]] glm::vec3 getForwardVector() const noexcept;
        [[nodiscard]] glm::vec3 getUpVector() const noexcept;
        [[nodiscard]] glm::vec3 getRightVector() const noexcept;

        [[nodiscard]] nlohmann::json toJson() const override;
        void fromJson(const nlohmann::json& json) override;

    protected:
        void recalculateMatrices() noexcept;
        void tryToRecalculateMatrices() noexcept;

    protected:
        glm::mat4 _cachedModelMatrix = glm::mat4(1.f);
        glm::vec3 _scale = glm::vec3(1.f, 1.f, 1.f);
        glm::vec3 _origin{};
        GPos3 _position{};
        glm::vec3 _rotation{};
        bool _isDirtyModelMatrix = true;
    };
} // namespace SW