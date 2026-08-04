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

#pragma once

#include "Core/Size.h"
#include "Entities/Actor.h"

namespace Core
{
    ENUM_CLASS();
    enum class CameraType : uint8_t
    {
        Perspective,
        Orthographic,
    };

    CLASS();
    class BaseCamera : public Actor
    {
        ECS_DECL(BaseCamera, Core::Actor);

    public:
        BaseCamera(const BaseCamera&) = default;
        BaseCamera(BaseCamera&&) noexcept = default;
        BaseCamera& operator=(const BaseCamera&) = default;
        BaseCamera& operator=(BaseCamera&&) noexcept = default;
        ~BaseCamera() override = default;

        [[nodiscard]] virtual const glm::mat4& getMatrix() = 0;
        [[nodiscard]] virtual glm::vec3 putMouseRay(float length) = 0;
        [[nodiscard]] virtual CameraType getType() const noexcept = 0;

        [[nodiscard]] const glm::mat4& getCachedProjectionMatrix() { return _cachedProjMatrix; }

        [[nodiscard]] FSize2 getOutputFrameSize();

        [[nodiscard]] glm::vec3 getGlobalPos() const noexcept { return _worldPos; }
        [[nodiscard]] glm::vec3 getGlobalRotation() const noexcept { return _worldRotation; }

        void setNear(float value) noexcept;
        [[nodiscard]] float getNear() const noexcept { return _near; }

        void setFar(float value) noexcept;
        [[nodiscard]] float getFar() const noexcept { return _far; }

        [[nodiscard]] StringAtom getCacheHash() const override;

        void tryToRecalculateCameraMatrices();

    protected:
        void recalculateCameraMatrices();

    protected:
        glm::mat4 _cachedProjMatrix = glm::mat4(1.f);
        glm::mat4 _cachedCalculatedMatrix = glm::mat4(1.f);
        glm::vec3 _worldRotation = glm::vec3(0.f);
        glm::vec3 _worldPos = glm::vec3(0.f);

        FIELD();
        float _far = 10'000.f;
        FIELD();
        float _near = 0.1f;

        bool _isDirtyProjMatrix = true;
    };

    CLASS();
    class OrthographicCamera : public BaseCamera
    {
        ECS_DECL(OrthographicCamera, Core::BaseCamera);

    public:
        OrthographicCamera(OrthographicCamera&&) noexcept = default;
        OrthographicCamera& operator=(const OrthographicCamera&) = default;
        OrthographicCamera& operator=(OrthographicCamera&&) noexcept = default;
        ~OrthographicCamera() override = default;

        [[nodiscard]] const glm::mat4& getMatrix() override;
        [[nodiscard]] glm::vec3 putMouseRay(float length) override;

        [[nodiscard]] CameraType getType() const noexcept override
        {
            return CameraType::Orthographic;
        }

    protected:
        FIELD();
        glm::vec2 _topLeft = glm::vec2(0.f, 0.f);
        FIELD();
        glm::vec2 _bottomRight = glm::vec2(800.f, 600.f);
    };

    CLASS();
    class PerspectiveCamera : public BaseCamera
    {
        ECS_DECL(PerspectiveCamera, Core::BaseCamera);

    public:
        constexpr static float minFov = 5.f;
        constexpr static float maxFov = 175.f;

    public:
        PerspectiveCamera(const PerspectiveCamera&) = default;
        PerspectiveCamera(PerspectiveCamera&&) noexcept = default;
        PerspectiveCamera& operator=(const PerspectiveCamera&) = default;
        PerspectiveCamera& operator=(PerspectiveCamera&&) noexcept = default;
        ~PerspectiveCamera() override = default;

        [[nodiscard]] const glm::mat4& getMatrix() override;

        void lookAt(const glm::vec3& targetPosition);

        void setAspect(FSize2 size) noexcept;
        [[nodiscard]] FSize2 getAspect() const noexcept { return _aspect; }

        void setFov(float fov) noexcept;
        [[nodiscard]] float getFov() const noexcept { return _fov; }

        [[nodiscard]] glm::vec3 putMouseRay(float length) override;

        [[nodiscard]] CameraType getType() const noexcept override
        {
            return CameraType::Perspective;
        }

    protected:
    protected:
        FIELD();
        Core::FSize2 _aspect = Core::FSize2{ 1.f, 1.f };
        FIELD();
        float _fov = 75.f;
    };

} // namespace Core

#include "Camera.generated.h" // added by the code generator. Better don't move it.
