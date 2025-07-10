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

#include "BaseSceneObject.h"

#define GLM_FORCE_RADIANS
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"

namespace SW
{

    void BaseSceneObject::setPosition(const glm::vec3& position) noexcept
    {
        _position = position;
        _areDirtyMatrices = true;
    }

    glm::vec3 BaseSceneObject::getPosition() const noexcept
    {
        return _position + _origin;
    }

    void BaseSceneObject::moveForward(float offset) noexcept
    {
        _position += offset * getForwardVector();
        _areDirtyMatrices = true;
    }

    void BaseSceneObject::moveRight(float offset) noexcept
    {
        _position += -offset * getRightVector();
        _areDirtyMatrices = true;
    }

    void BaseSceneObject::moveUp(float offset) noexcept
    {
        _position += offset * getUpVector();
        _areDirtyMatrices = true;
    }

    void BaseSceneObject::setRotation(const glm::vec3& rotation) noexcept
    {
        _rotation = rotation;
        _areDirtyMatrices = true;

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

    void BaseSceneObject::rotate(const glm::vec3& value) noexcept
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

        _areDirtyMatrices = true;
    }

    const glm::vec3& BaseSceneObject::getRotation() const noexcept
    {
        return _rotation;
    }

    glm::vec3& BaseSceneObject::getRotation() noexcept
    {
        return _rotation;
    }

    void BaseSceneObject::setRotationX(float x) noexcept
    {
        _rotation.x = x;
        _areDirtyMatrices = true;
    }

    void BaseSceneObject::rotateX(float x) noexcept
    {
        _rotation += x;
        _areDirtyMatrices = true;
        while (_rotation.x >= 360.f)
        {
            _rotation.x -= 360.f;
        }
    }

    float BaseSceneObject::getRotationX() const noexcept
    {
        return _rotation.x;
    }

    void BaseSceneObject::setRotationY(float y) noexcept
    {
        _rotation.y = y;
        _areDirtyMatrices = true;
        while (_rotation.y >= 360.f)
        {
            _rotation.y -= 360.f;
        }
    }

    void BaseSceneObject::rotateY(float y) noexcept
    {
        _rotation.y += y;
        _areDirtyMatrices = true;
        while (_rotation.y >= 360.f)
        {
            _rotation.y -= 360.f;
        }
    }

    float BaseSceneObject::getRotationY() const noexcept
    {
        return _rotation.y;
    }

    void BaseSceneObject::setRotationZ(float z) noexcept
    {
        _rotation.z = z;
        _areDirtyMatrices = true;
        while (_rotation.z >= 360.f)
        {
            _rotation.z -= 360.f;
        }
    }

    void BaseSceneObject::rotateZ(float z) noexcept
    {
        _rotation.z += z;
        _areDirtyMatrices = true;
        while (_rotation.z >= 360.f)
        {
            _rotation.z -= 360.f;
        }
    }

    float BaseSceneObject::getRotationZ() const noexcept
    {
        return _rotation.z;
    }

    void BaseSceneObject::setScale(const glm::vec3& value) noexcept
    {
        _scale = value;
        _areDirtyMatrices = true;
    }

    void BaseSceneObject::scale(const glm::vec3& value) noexcept
    {
        _scale += value;
        _areDirtyMatrices = true;
    }

    glm::vec3 BaseSceneObject::getScale() const noexcept
    {
        return _scale;
    }

    glm::vec3 BaseSceneObject::getForwardVector() const noexcept
    {
        auto r = glm::vec2(_rotation.x, _rotation.y);

        // clang-format off
        return glm::normalize(glm::vec3{
            cos(r.x) * -sin(r.y),
            sin(r.x),
            cos(r.x) * cos(r.y)
        });
        // clang-format on
    }

    glm::vec3 BaseSceneObject::getUpVector() const noexcept
    {
        auto r = glm::vec2(_rotation.x, _rotation.y);
        r.x += -90.f;

        // clang-format off
        return glm::normalize(glm::vec3{
            cos(r.x) * -sin(r.y),
            sin(r.x),
            cos(r.x) * cos(r.y)
        });
        // clang-format on
    }

    glm::vec3 BaseSceneObject::getRightVector() const noexcept
    {
        return glm::normalize(glm::cross(getForwardVector(), getUpVector()));
    }

    void BaseSceneObject::recalculateMatrices() noexcept
    {
        if (_areDirtyMatrices)
        {
            _cachedModelMatrix = glm::mat4(1.f);

            _cachedModelMatrix = glm::translate(_cachedModelMatrix, _position);

            _cachedModelMatrix = glm::rotate(_cachedModelMatrix, glm::radians(_rotation.x),
                                             glm::vec3(1.f, 0.f, 0.f));
            _cachedModelMatrix = glm::rotate(_cachedModelMatrix, glm::radians(_rotation.y),
                                             glm::vec3(0.f, 1.f, 0.f));
            _cachedModelMatrix = glm::rotate(_cachedModelMatrix, glm::radians(_rotation.z),
                                             glm::vec3(0.f, 0.f, 1.f));
            _cachedModelMatrix = glm::translate(_cachedModelMatrix, -_origin);

            _cachedModelMatrix = glm::translate(_cachedModelMatrix, _origin);
            _cachedModelMatrix = glm::scale(_cachedModelMatrix, _scale);
            _cachedModelMatrix = glm::translate(_cachedModelMatrix, -_origin);

            _areDirtyMatrices = false;
        }
    }

    void BaseSceneObject::setOrigin(const glm::vec3& origin) noexcept
    {
        _origin = origin;
        _areDirtyMatrices = true;
    }

    const glm::vec3& BaseSceneObject::getOrigin() const noexcept
    {
        return _origin;
    }

} // namespace SW