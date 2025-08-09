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

#include "Transformable.h"

#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/string_cast.hpp"

#include <iostream>

namespace SW
{

    void Transformable::setPosition(const GPos3& position) noexcept
    {
        _position = position;
        _isDirtyModelMatrix = true;
    }

    void Transformable::addPosition(const GPos3& position) noexcept
    {
        setPosition(GPos3(_position + position));
    }

    GPos3 Transformable::getPosition() const noexcept
    {
        return GPos3{ _position + _origin };
    }

    void Transformable::moveForward(float offset) noexcept
    {
        _position += offset * getForwardVector();
        _isDirtyModelMatrix = true;
    }

    void Transformable::moveRight(float offset) noexcept
    {
        _position += offset * getRightVector();
        _isDirtyModelMatrix = true;
    }

    void Transformable::moveUp(float offset) noexcept
    {
        _position += -offset * getUpVector();
        _isDirtyModelMatrix = true;
    }

    void Transformable::setRotation(const glm::vec3& rotation) noexcept
    {
        _rotation = rotation;
        _isDirtyModelMatrix = true;

        while (_rotation.y >= 360.f)
        {
            _rotation.y -= 360.f;
        }
        while (_rotation.x >= 360.f)
        {
            _rotation.x -= 360.f;
        }
        while (_rotation.z >= 360.f)
        {
            _rotation.z -= 360.f;
        }
    }

    void Transformable::addRotation(const glm::vec3& rotation) noexcept
    {
        _rotation += rotation;
        _isDirtyModelMatrix = true;
    }

    void Transformable::rotate(const glm::vec3& value) noexcept
    {
        _rotation.y += value.x;
        _rotation.x += value.y;
        _rotation.z += value.z;

        while (_rotation.y >= 360.f)
        {
            _rotation.y -= 360.f;
        }
        while (_rotation.x >= 360.f)
        {
            _rotation.x -= 360.f;
        }
        while (_rotation.z >= 360.f)
        {
            _rotation.z -= 360.f;
        }

        _isDirtyModelMatrix = true;
    }

    const glm::vec3& Transformable::getRotation() const noexcept
    {
        return _rotation;
    }

    void Transformable::setRotationX(float x) noexcept
    {
        _rotation.x = x;
        _isDirtyModelMatrix = true;
    }

    void Transformable::rotateX(float x) noexcept
    {
        _rotation.x += x;
        _isDirtyModelMatrix = true;
        while (_rotation.x >= 360.f)
        {
            _rotation.x -= 360.f;
        }
    }

    float Transformable::getRotationX() const noexcept
    {
        return _rotation.x;
    }

    void Transformable::setRotationY(float y) noexcept
    {
        _rotation.y = y;
        _isDirtyModelMatrix = true;
        while (_rotation.y >= 360.f)
        {
            _rotation.y -= 360.f;
        }
    }

    void Transformable::rotateY(float y) noexcept
    {
        _rotation.y += y;
        _isDirtyModelMatrix = true;
        while (_rotation.y >= 360.f)
        {
            _rotation.y -= 360.f;
        }
    }

    float Transformable::getRotationY() const noexcept
    {
        return _rotation.y;
    }

    void Transformable::setRotationZ(float z) noexcept
    {
        _rotation.z = z;
        _isDirtyModelMatrix = true;
        while (_rotation.z >= 360.f)
        {
            _rotation.z -= 360.f;
        }
    }

    void Transformable::rotateZ(float z) noexcept
    {
        _rotation.z += z;
        _isDirtyModelMatrix = true;
        while (_rotation.z >= 360.f)
        {
            _rotation.z -= 360.f;
        }
    }

    float Transformable::getRotationZ() const noexcept
    {
        return _rotation.z;
    }

    void Transformable::setScale(const glm::vec3& value) noexcept
    {
        _scale = value;
        _isDirtyModelMatrix = true;
    }

    void Transformable::addScale(const glm::vec3& value) noexcept
    {
        _scale += value;
        _isDirtyModelMatrix = true;
    }

    void Transformable::scale(const glm::vec3& value) noexcept
    {
        _scale += value;
        _isDirtyModelMatrix = true;
    }

    glm::vec3 Transformable::getScale() const noexcept
    {
        return _scale;
    }

    glm::vec3 Transformable::getForwardVector() const noexcept
    {
        const auto r = glm::vec2(glm::radians(_rotation.x), glm::radians(_rotation.y));

        // clang-format off
        return glm::normalize(glm::vec3{
            cos(r.x) * -sin(r.y),
            sin(r.x),
            cos(r.x) * cos(r.y)
        });
        // clang-format on
    }

    glm::vec3 Transformable::getUpVector() const noexcept
    {
        auto r = glm::vec2(glm::radians(_rotation.x), glm::radians(_rotation.y));
        r.x += glm::radians(-90.f);

        // clang-format off
        return glm::normalize(glm::vec3{
            cos(r.x) * -sin(r.y),
            sin(r.x),
            cos(r.x) * cos(r.y)
        });
        // clang-format on
    }

    glm::vec3 Transformable::getRightVector() const noexcept
    {
        return glm::normalize(glm::cross(getForwardVector(), getUpVector()));
    }

    nlohmann::json Transformable::toJson() const
    {
        nlohmann::json json;

        json["position"] = _position;
        json["origin"] = _origin;
        json["rotation"] = _rotation;
        json["scale"] = _scale;

        return json;
    }

    void Transformable::fromJson(const nlohmann::json& json, bool isIgnoreChildren /* = false*/)
    {
        _position = json["position"];
        _origin = json["origin"];
        _rotation = json["rotation"];
        _scale = json["scale"];

        _isDirtyModelMatrix = true;
    }

    void Transformable::recalculateMatrices(const glm::mat4& mat) noexcept
    {
        _cachedModelMatrix = mat;

        _cachedModelMatrix = glm::translate(_cachedModelMatrix, _position);

        _cachedModelMatrix
            = glm::rotate(_cachedModelMatrix, glm::radians(_rotation.x), glm::vec3(1.f, 0.f, 0.f));
        _cachedModelMatrix
            = glm::rotate(_cachedModelMatrix, glm::radians(_rotation.y), glm::vec3(0.f, 1.f, 0.f));
        _cachedModelMatrix
            = glm::rotate(_cachedModelMatrix, glm::radians(_rotation.z), glm::vec3(0.f, 0.f, 1.f));
        _cachedModelMatrix = glm::translate(_cachedModelMatrix, -_origin);

        _cachedModelMatrix = glm::translate(_cachedModelMatrix, _origin);
        _cachedModelMatrix = glm::scale(_cachedModelMatrix, _scale);
        _cachedModelMatrix = glm::translate(_cachedModelMatrix, -_origin);

        _isDirtyModelMatrix = false;
    }

    void Transformable::tryToRecalculateMatrices(const glm::mat4& mat) noexcept
    {
        if (_isDirtyModelMatrix)
        {
            recalculateMatrices(mat);
        }
    }

    void Transformable::setOrigin(const glm::vec3& origin) noexcept
    {
        _origin = origin;
        _isDirtyModelMatrix = true;
    }

    void Transformable::addOrigin(const glm::vec3& origin) noexcept
    {
        _origin += origin;
        _isDirtyModelMatrix = true;
    }

    const glm::vec3& Transformable::getOrigin() const noexcept
    {
        return _origin;
    }

} // namespace SW