/*
 * MIT License
 *
 * Copyright (c) 2018-2027 Valerii Koniushenko
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

#include "GameplaySystem/Camera.h"

#include "GameplaySystem/Framework/GameInstance.h"
#include "Graphics/Window.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/string_cast.hpp"

#include <cmath>

namespace Core
{
    ECS_IMPL(BaseCamera);
    ECS_IMPL(OrthographicCamera);
    ECS_IMPL(PerspectiveCamera);

    void BaseCamera::recalculateCameraMatrices()
    {
        _worldPos = glm::vec3(0);
        _worldRotation = glm::vec3(0);

        BaseComponent* p = this;
        while (p)
        {
            if (auto* t = dynamic_cast<Transformable*>(p))
            {
                _worldPos += t->getPosition();
                _worldRotation += t->getRotation();
            }
            p = p->getParent();
        }

        auto& mat = _cachedModelMatrix;
        mat = glm::mat4(1.f);

        mat = glm::rotate(mat, glm::radians(_worldRotation.x), glm::vec3(1.f, 0.f, 0.f));
        mat = glm::rotate(mat, glm::radians(_worldRotation.y), glm::vec3(0.f, 1.f, 0.f));
        mat = glm::rotate(mat, glm::radians(_worldRotation.z), glm::vec3(0.f, 0.f, 1.f));

        mat = glm::translate(mat, _worldPos * -1.f);
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

    StringAtom BaseCamera::getCacheHash() const
    {
        return getComponentType() + "_" + getComponentName();
    }

    BaseCamera::~BaseCamera()
    {
        if (gGameInstance && this == GetWorld()->currentCamera)
        {
            gGameInstance->resetCamera();
        }
    }

    FSize2 BaseCamera::getOutputFrameSize()
    {
        if (gGameInstance->renderMode == GameInstance::RenderMode::Editor)
        {
            return static_cast<FSize2>(gGameInstance->gameViewport.getRenderSize());
        }

        return static_cast<FSize2>(GetWindow().getSize());
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

    const glm::mat4& OrthographicCamera::getMatrix()
    {
        if (_isDirtyProjMatrix)
        {
            _cachedProjMatrix
                = glm::ortho(_topLeft.x, _bottomRight.x, _topLeft.y, _bottomRight.y, _near, _far);

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

    glm::vec3 OrthographicCamera::putMouseRay(float length)
    {
        const auto mouse = Mouse::GetInViewportPosition();

        const auto frame = getOutputFrameSize();
        const float x = (2.0f * mouse.x / frame.width) - 1.0f;
        const float y = 1.0f - (2.0f * mouse.y / frame.height);

        const auto rayClip = glm::vec4(x, y, -1.0f, 1.0f);
        glm::vec4 rayEye = glm::inverse(_cachedProjMatrix) * rayClip;
        rayEye.z = -1.0f;
        rayEye.w = 1.0f;

        const auto rayOriginWorld = glm::vec3(glm::inverse(_cachedModelMatrix) * rayEye);

        constexpr glm::vec4 forwardEye(0.0f, 0.0f, -1.0f, 0.0f);
        const glm::vec3 ray_world
            = glm::normalize(glm::vec3(glm::inverse(_cachedModelMatrix) * forwardEye));

        return rayOriginWorld + ray_world * length;
    }

    void OrthographicCamera::setTopLeft(const glm::vec2& topLeft) noexcept
    {
        _topLeft = topLeft;
        _isDirtyProjMatrix = true;
    }

    void OrthographicCamera::setBottomRight(const glm::vec2& bottomRight) noexcept
    {
        _bottomRight = bottomRight;
        _isDirtyProjMatrix = true;
    }

    const glm::mat4& PerspectiveCamera::getMatrix()
    {
        if (_isDirtyProjMatrix)
        {
            _cachedProjMatrix
                = glm::perspective(glm::radians(_fov), _aspect.width / _aspect.height, _near, _far);

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

    void PerspectiveCamera::lookAt(const glm::vec3& targetPosition)
    {
        const auto direction = glm::normalize(targetPosition - getPosition());

        const auto yaw = static_cast<float>(glm::degrees(atan2(-direction.x, direction.z)));
        const auto pitch = static_cast<float>(glm::degrees(asin(direction.y)));

        setRotation({ pitch, yaw, 0.0f });
    }

    void PerspectiveCamera::setAspect(FSize2 size) noexcept
    {
        _aspect = size;
        _isDirtyProjMatrix = true;
    }

    void PerspectiveCamera::setFov(float fov) noexcept
    {
        _fov = std::clamp(fov, minFov, maxFov);
        _isDirtyProjMatrix = true;
    }

    glm::vec3 PerspectiveCamera::putMouseRay(float length)
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

} // namespace Core
