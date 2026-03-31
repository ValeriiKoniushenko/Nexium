/*
 * MIT License
 *
 * Copyright (c) 2018-2025 Valerii Koniushenko
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "Transformable.h"

#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/string_cast.hpp"

namespace Core
{
    void Transformable::setPosition(const GPos3& position) noexcept
    {
        _position = position;
        _isDirtyModelMatrix = true;
        onDirtyMatrix();
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
        onDirtyMatrix();
    }

    void Transformable::moveRight(float offset) noexcept
    {
        auto tmp = getRightVector();
        _position += offset * tmp;
        _isDirtyModelMatrix = true;
        onDirtyMatrix();
    }

    void Transformable::moveUp(float offset) noexcept
    {
        _position += -offset * getUpVector();
        _isDirtyModelMatrix = true;
        onDirtyMatrix();
    }

    void Transformable::setRotation(const glm::vec3& value) noexcept
    {
        setRotationX(value.x);
        setRotationY(value.y);
        setRotationZ(value.z);
    }

    void Transformable::rotate(const glm::vec3& value) noexcept
    {
        setRotationX(_rotation.x + value.x);
        setRotationY(_rotation.y + value.y);
        setRotationZ(_rotation.z + value.z);
    }

    const glm::vec3& Transformable::getRotation() const noexcept
    {
        return _rotation;
    }

    void Transformable::setRotationX(float x) noexcept
    {
        _rotation.x = x;
        _isDirtyModelMatrix = true;
        onDirtyMatrix();
    }

    void Transformable::rotateX(float x) noexcept
    {
        setRotationX(_rotation.x + x);
    }

    float Transformable::getRotationX() const noexcept
    {
        return _rotation.x;
    }

    void Transformable::setRotationY(float y) noexcept
    {
        _rotation.y = std::fmod(y, 360.f);
        _isDirtyModelMatrix = true;
        onDirtyMatrix();
    }

    void Transformable::rotateY(float y) noexcept
    {
        setRotationY(_rotation.y + y);
    }

    float Transformable::getRotationY() const noexcept
    {
        return _rotation.y;
    }

    void Transformable::setRotationZ(float z) noexcept
    {
        _rotation.z = std::fmod(z, 360.f);
        _isDirtyModelMatrix = true;
        onDirtyMatrix();
    }

    void Transformable::rotateZ(float z) noexcept
    {
        setRotationZ(_rotation.z + z);
    }

    float Transformable::getRotationZ() const noexcept
    {
        return _rotation.z;
    }

    void Transformable::setScale(const glm::vec3& value) noexcept
    {
        _scale = value;
        _isDirtyModelMatrix = true;
        onDirtyMatrix();
    }

    void Transformable::addScale(const glm::vec3& value) noexcept
    {
        _scale += value;
        _isDirtyModelMatrix = true;
        onDirtyMatrix();
    }

    void Transformable::scale(const glm::vec3& value) noexcept
    {
        _scale += value;
        _isDirtyModelMatrix = true;
        onDirtyMatrix();
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

    void Transformable::ioFieldsUpdate(DataStream& out)
    {
        auto stream = out.dedicatedNesting("Transformable");

        stream.field("position", _position);
        stream.field("origin", _origin);
        stream.field("rotation", _rotation);
        stream.field("scale", _scale);

        if (stream.getMode() == DataStream::Mode::Input)
        {
            _isDirtyModelMatrix = true;
            onDirtyMatrix();
        }
    }

    void Transformable::recalculateMatrices(const glm::mat4& mat)
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

    void Transformable::tryToRecalculateMatrices(const glm::mat4& mat)
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
        onDirtyMatrix();
    }

    void Transformable::addOrigin(const glm::vec3& origin) noexcept
    {
        _origin += origin;
        _isDirtyModelMatrix = true;
        onDirtyMatrix();
    }

    const glm::vec3& Transformable::getOrigin() const noexcept
    {
        return _origin;
    }

    StringAtom Transformable::getCacheHash() const
    {
        return "Transformable"_atom;
    }
} // namespace Core