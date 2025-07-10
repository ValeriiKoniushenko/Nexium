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

#include "RawGraphics/Window.h"

// #define GLM_FORCE_RADIANS
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"

namespace SW
{
    BaseCamera::BaseCamera()
    {
    }

    void BaseCamera::rotate(const glm::vec3& value_)
    {
        throw "not implemented";
        // SceneObject::rotate(value); - Don't do this
        // auto value = value_;
        // value.x /= sensitive_.x;
        // value.y /= sensitive_.y;
        //
        // rotation_.y += value.x;
        // rotation_.x += value.y;
        //
        // if (rotation_.x > maxPitch_)
        // {
        //     rotation_.x = maxPitch_;
        // }
        // if (rotation_.x < -maxPitch_)
        // {
        //     rotation_.x = -maxPitch_;
        // }
        //
        // while (rotation_.y >= 360.f)
        // {
        //     rotation_.y -= 360.f;
        // }
        //
        // matricesAreDirty_ = true;
    }

    const glm::mat4& BaseCamera::getMatrix()
    {
        auto [width, height] = GetWindow().getSize();
        _cachedProjMatrix = glm::perspective(
            fov_, static_cast<float>(width) / static_cast<float>(height), near_, far_);

        _cachedCalculatedMatrix = _cachedProjMatrix * _cachedViewMatrix;
        return _cachedCalculatedMatrix;
    }

    void BaseCamera::setFov(float fov)
    {
        fov_ = fov;
    }

    float BaseCamera::getFov() const
    {
        return fov_;
    }

    void BaseCamera::setNear(float value)
    {
        near_ = value;
    }

    float BaseCamera::getNear() const
    {
        return near_;
    }

    void BaseCamera::setFar(float value)
    {
        far_ = value;
    }

    float BaseCamera::getFar() const
    {
        return far_;
    }

    void BaseCamera::setSensitive(glm::vec2 value)
    {
        _sensitive = value;
    }

    glm::vec2 BaseCamera::getSensitive() const
    {
        return _sensitive;
    }

    glm::vec3 BaseCamera::getPosition() const
    {
        return _position;
    }

    void BaseCamera::setPosition(const glm::vec3& position)
    {
        _position = position;
    }

} // namespace SW