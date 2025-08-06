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

#include "Camera.h"

#include "Graphics/Window.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"

namespace SW
{

    const glm::mat4& BaseCamera::getMatrix()
    {
        if (_isDirtyProjMatrix)
        {
            _cachedProjMatrix
                = glm::perspective(glm::radians(_fov), _size.width / _size.height, _near, _far);

            _cachedCalculatedMatrix
                = _cachedProjMatrix * _cachedModelMatrix; // in such context Model == View

            _isDirtyProjMatrix = false;
        }

        if (_isDirtyModelMatrix)
        {
            recalculateCameraMatrices();

            _cachedCalculatedMatrix
                = _cachedProjMatrix * _cachedModelMatrix; // in such context Model == View
        }

        return _cachedCalculatedMatrix;
    }

    void BaseCamera::lookAt(const glm::vec3& targetPosition)
    {
        const auto direction = glm::normalize(targetPosition - getPosition());

        const float yaw = glm::degrees(atan2(-direction.x, direction.z));
        const float pitch = glm::degrees(asin(direction.y));

        setRotation({ pitch, yaw, 0.0f });
    }

    void BaseCamera::setFrameSize(Core::FSize2 size) noexcept
    {
        _size = size;
        _isDirtyProjMatrix = true;
    }

    void BaseCamera::setFov(float fov) noexcept
    {
        _fov = std::clamp(fov, 30.f, 180.f);
        _isDirtyProjMatrix = true;
    }

    void BaseCamera::setNear(float value) noexcept
    {
        _near = value;
        _isDirtyProjMatrix = true;
    }

    void BaseCamera::setFar(float value) noexcept
    {
        _far = value;
        _isDirtyProjMatrix = true;
    }

    void BaseCamera::setSensitive(glm::vec2 value) noexcept
    {
        _sensitive = value;
        _isDirtyProjMatrix = true;
    }

    void BaseCamera::yaw(float y)
    {
        rotateY(y);
    }

    void BaseCamera::pitch(float x)
    {
        if (fabs(_rotation.x + x) > 90.f)
        {
            return;
        }

        rotateX(x);
    }

    void BaseCamera::yawAndPitch(glm::vec2 xy)
    {
        yaw(xy.x);
        pitch(xy.y);
    }

    nlohmann::json BaseCamera::toJson() const
    {
        auto json = Actor::toJson();

        json["viewport"] = _size;
        json["fov"] = _fov;
        json["far"] = _far;
        json["near"] = _near;
        return json;
    }

    void BaseCamera::fromJson(const nlohmann::json& json, bool isIgnoreChildren /* = false*/)
    {
        Actor::fromJson(json, isIgnoreChildren);
        std::cout << json.dump(4) << std::endl;
        _size = json["viewport"].get<decltype(_size)>();
        _fov = json["fov"].get<decltype(_fov)>();
        _far = json["far"].get<decltype(_far)>();
        _near = json["near"].get<decltype(_near)>();
    }

    Core::StringAtom BaseCamera::getCacheHash() const
    {
        return "BaseCamera";
    }

    nlohmann::json BaseCamera::toCacheData() const
    {
        return toJson();
    }

    void BaseCamera::fromCacheData(const nlohmann::json& data)
    {
        fromJson(data);
    }

    void BaseCamera::recalculateCameraMatrices()
    {
        auto& mat = _cachedModelMatrix;

        mat = glm::mat4(1.f);

        mat = glm::rotate(mat, glm::radians(_rotation.x), glm::vec3(1.f, 0.f, 0.f));
        mat = glm::rotate(mat, glm::radians(_rotation.y), glm::vec3(0.f, 1.f, 0.f));
        mat = glm::rotate(mat, glm::radians(_rotation.z), glm::vec3(0.f, 0.f, 1.f));

        mat = glm::translate(mat, _position);
        mat = glm::translate(mat, _origin);

        _isDirtyModelMatrix = false;
    }

    void BaseCamera::tryToRecalculateCameraMatrices()
    {
        if (_isDirtyModelMatrix)
        {
            recalculateCameraMatrices();
        }
    }

} // namespace SW