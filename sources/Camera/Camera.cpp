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

#include "GameplaySystem/Framework/GameInstance.h"
#include "Graphics/Window.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"

#include <cmath>

namespace Core
{
    ECS_REGISTER_NEW_TYPE(BaseCamera)

    const glm::mat4& BaseCamera::getMatrix()
    {
        if (_isDirtyProjMatrix)
        {
            _cachedProjMatrix = glm::perspective(glm::radians(_fov),
                                                 _frameSize.width / _frameSize.height, _near, _far);

            _cachedCalculatedMatrix
                = _cachedProjMatrix * _cachedModelMatrix; // in such a context Model == View

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

        const auto yaw = static_cast<float>(glm::degrees(atan2(-direction.x, direction.z)));
        const auto pitch = static_cast<float>(glm::degrees(asin(direction.y)));

        setRotation({ pitch, yaw, 0.0f });
    }

    void BaseCamera::setFrameSize(FSize2 size) noexcept
    {
        _frameSize = size;
        _isDirtyProjMatrix = true;
    }

    void BaseCamera::setFov(float fov) noexcept
    {
        _fov = std::clamp(fov, minFov, maxFov);
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

    void BaseCamera::yaw(float y)
    {
        rotateY(y);
    }

    void BaseCamera::pitch(float x)
    {
        if (static_cast<float>(std::fabs(_rotation.x + x)) > 90.f)
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

        json["frameSize"] = _frameSize;
        json["fov"] = _fov;
        json["far"] = _far;
        json["near"] = _near;
        return json;
    }

    void BaseCamera::fromJson(const nlohmann::json& json, bool isIgnoreChildren)
    {
        Actor::fromJson(json, isIgnoreChildren);

        if (json.contains("frameSize"))
        {
            _frameSize = json["frameSize"].get<decltype(_frameSize)>();
        }
        if (json.contains("fov"))
        {
            _fov = json["fov"].get<decltype(_fov)>();
        }
        if (json.contains("far"))
        {
            _far = json["far"].get<decltype(_far)>();
        }
        if (json.contains("near"))
        {
            _near = json["near"].get<decltype(_near)>();
        }
    }

    StringAtom BaseCamera::getCacheHash() const
    {
        return "BaseCamera"_atom;
    }

    nlohmann::json BaseCamera::toCacheData() const
    {
        return toJson();
    }

    void BaseCamera::fromCacheData(const nlohmann::json& data)
    {
        fromJson(data, false);
    }

    FSize2 BaseCamera::getOutputFrameSize()
    {
        if (gGameInstance->renderMode.cast() == GameInstance::RenderMode::Editor)
        {
            return static_cast<FSize2>(gGameInstance->renderToTextureObject.getRenderSize());
        }

        return static_cast<FSize2>(GetWindow().getSize());
    }

    glm::vec3 BaseCamera::putMouseRay(float length)
    {
        const auto mouse = Mouse::GetInViewportPosition();

        const auto frame = getOutputFrameSize();
        const float x = (2.0f * mouse.x / frame.width) - 1.0f;
        const float y = 1.0f - (2.0f * mouse.y / frame.height);

        const auto rayClip = glm::vec4(x, y, -1.0f, 1.0f);
        glm::vec4 rayEye = glm::inverse(_cachedProjMatrix) * rayClip;
        rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);
        const glm::vec3 ray_world
            = glm::normalize(glm::vec3(glm::inverse(_cachedModelMatrix) * rayEye));

        return _position + ray_world * length;
    }

    void BaseCamera::recalculateCameraMatrices()
    {
        auto& mat = _cachedModelMatrix;

        mat = glm::mat4(1.f);

        mat = glm::rotate(mat, glm::radians(_rotation.x), glm::vec3(1.f, 0.f, 0.f));
        mat = glm::rotate(mat, glm::radians(_rotation.y), glm::vec3(0.f, 1.f, 0.f));
        mat = glm::rotate(mat, glm::radians(_rotation.z), glm::vec3(0.f, 0.f, 1.f));

        mat = glm::translate(mat, _position * -1.f);
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

} // namespace Core