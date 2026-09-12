// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "Transformable.h"

#include "glm/ext/matrix_transform.hpp"

namespace NX
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

    void Transformable::yaw(float y)
    {
        rotateY(y);
    }

    void Transformable::pitch(float x)
    {
        if (static_cast<float>(std::fabs(_rotation.x + x))
            > 90.f) // NOLINT(readability-redundant-casting)
        {
            return;
        }

        rotateX(x);
    }

    void Transformable::yawAndPitch(glm::vec2 xy)
    {
        yaw(xy.x);
        pitch(xy.y);
    }

    void Transformable::setTransformations(const Transformable& trans)
    {
        setScale(trans.getScale());
        setOrigin(trans.getOrigin());
        setPosition(trans.getPosition());
        setRotation(trans.getRotation());
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
    void Transformable::onPostDeserialize(Transformable* obj, const RLogsCollector& logs)
    {
        _isDirtyModelMatrix = true;
        onDirtyMatrix();
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

} // namespace NX
